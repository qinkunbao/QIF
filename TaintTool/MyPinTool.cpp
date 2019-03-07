//This is a Pin tool that will split the execution trace into per-message traces by monitoring the program's
//use of netwokring functions that read or write from sockets.
//Output:
//1. Taintdata execution traces
//2. Function Each function in the memory address

#include <iostream>
#include <string>
#include <list>
#include <map>
#include "pin.H"


bool recv_start = false;
std::map<ADDRINT, string> opcmap;
FILE *fp, *rtn_fp;

// We don't combine rtn_recv and rtn_send together becasue we need compilers try to make them incline
VOID static rtn_recv(char *name, char *rtn_name, unsigned int socket, UINT64 buf, int len, int flags) {
	cout << "Function: " << name << " found. " << "RTN: " << rtn_name << " Socket: " << socket << endl;
	if (recv_start == FALSE) {
		recv_start = TRUE;
		fprintf(fp, "Start; %x; %x; \n", buf, len);
		return;
	}
	if ((recv_start == TRUE)) {
		recv_start = FALSE;
		fprintf(fp, "END\n");
	}
}

VOID static rtn_send(char *name, char *rtn_name, unsigned int socket, UINT64 buf, int len, int flags) {
	cout << "Function: " << name << " found. " << "RTN: " << rtn_name << " Socket: " << socket << endl;
	if ((recv_start == TRUE)) {
		recv_start = FALSE;
	}
	fprintf(fp, "Function name: %s RTN_Name: %s Buffer_address: %x Length: %x \n", name, rtn_name, buf, len);
}


INT32 Usage()
{
	std::cerr << "Taint Analysis for network data";
	cerr << endl << endl;
	cerr << endl;
	return -1;
}

void static getctx(ADDRINT addr, CONTEXT *fromctx, ADDRINT memaddr)
{
	//Only collect traces and a recv function
	if (recv_start == FALSE)
		return;
	fprintf(fp, "%x;%s;%x,%x,%x,%x,%x,%x,%x,%x,%x,\n", addr, opcmap[addr].c_str(),
		PIN_GetContextReg(fromctx, REG_EAX),
		PIN_GetContextReg(fromctx, REG_EBX),
		PIN_GetContextReg(fromctx, REG_ECX),
		PIN_GetContextReg(fromctx, REG_EDX),
		PIN_GetContextReg(fromctx, REG_ESI),
		PIN_GetContextReg(fromctx, REG_EDI),
		PIN_GetContextReg(fromctx, REG_ESP),
		PIN_GetContextReg(fromctx, REG_EBP),
		memaddr);
}


VOID Instruction(INS ins, VOID *v) {

	if (INS_OperandCount(ins) == 1)
		return;
	ADDRINT addr = INS_Address(ins);
	if (opcmap.find(addr) == opcmap.end()) {
		opcmap.insert(std::pair<ADDRINT, string>(addr, INS_Disassemble(ins)));
	}

	if (INS_IsMemoryRead(ins)) {
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)getctx, IARG_INST_PTR, IARG_CONST_CONTEXT, IARG_MEMORYREAD_EA, IARG_END);
	}


	else if (INS_IsMemoryWrite(ins)) {
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)getctx, IARG_INST_PTR, IARG_CONST_CONTEXT, IARG_MEMORYWRITE_EA, IARG_END);
	}

	else {
		INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)getctx, IARG_INST_PTR, IARG_CONST_CONTEXT, IARG_ADDRINT, 0, IARG_END);
	}
}

static VOID Routine(char* rtn_name, char* module_name, ADDRINT start)
{
	if (recv_start == FALSE)
		return;
	fprintf(rtn_fp, "%x; ", start);
	fprintf(rtn_fp, "%s;  ", module_name);
	fprintf(rtn_fp, "%s \n", rtn_name);

}

VOID Image(IMG img, VOID *v) {

	
	//if (IMG_IsMainExecutable(img))
	    //return;
	
	for (SYM sym = IMG_RegsymHead(img); SYM_Valid(sym); sym = SYM_Next(sym))
	{
		string undFuncName = PIN_UndecorateSymbolName(SYM_Name(sym), UNDECORATION_NAME_ONLY);
		RTN recvRtn = RTN_FindByAddress(IMG_LowAddress(img) + SYM_Value(sym));
		if (RTN_Valid(recvRtn))
		{
			RTN_Open(recvRtn);
			ADDRINT rtn_start = RTN_Address(recvRtn);
			ADDRINT rtn_end = rtn_start + RTN_Size(recvRtn);

			RTN_InsertCall(recvRtn, IPOINT_BEFORE, (AFUNPTR)Routine,
				IARG_PTR, RTN_Name(recvRtn).c_str(),
				IARG_PTR, IMG_Name(img).c_str(),
				IARG_ADDRINT, rtn_start,
				IARG_END);

			//if ((undFuncName == "recv") || (undFuncName == "send"))
			//{
				// Instrument to print the input argument value.

				if ((undFuncName == "recv")&&(IMG_Name(img).find("ws2_32.dll") != string::npos)) {
					RTN_InsertCall(recvRtn, IPOINT_BEFORE, (AFUNPTR)rtn_recv,
						IARG_ADDRINT, "recv",
						IARG_PTR, IMG_Name(img).c_str(),
						IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
						IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
						IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
						IARG_FUNCARG_ENTRYPOINT_VALUE, 3,
						IARG_END);
				}
				//else {
				//	RTN_InsertCall(recvRtn, IPOINT_BEFORE, (AFUNPTR)rtn_send,
				//		IARG_ADDRINT, "send",
				//		IARG_PTR, IMG_Name(img).c_str(),
				//		IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
				//		IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
				//		IARG_FUNCARG_ENTRYPOINT_VALUE, 2,
				//		IARG_END);
				//}
			//}
			RTN_Close(recvRtn);

		}
	}

}

static void Fini(INT32, void *v) {
	fclose(fp);
	fclose(rtn_fp);
}

int main(int argc, char *argv[])
{
	// Initialize pin & symbol manager
	PIN_InitSymbols();
	if (PIN_Init(argc, argv))
	{
		return Usage();
	}
	fp = fopen("Taint_data.txt", "w");
	rtn_fp = fopen("Function.txt", "w");
	// Register Image to be called to instrument functions.
	IMG_AddInstrumentFunction(Image, 0);
	INS_AddInstrumentFunction(Instruction, 0);
	PIN_AddFiniFunction(Fini, 0);

	// Never returns
	PIN_StartProgram();

	return 0;
}