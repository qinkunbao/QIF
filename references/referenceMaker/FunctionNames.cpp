/*
* A pin tool to record all routines except ntdll.dll
*
*/
#include <string>
#include <stdio.h>
#include <pin.H>
#include <map>
#include <iostream>


std::map<ADDRINT, string> opcmap;
FILE *fp, *ref;
bool start_trace = false;

static void startRTN(char* rtn_name)
{
    fprintf(fp, "%s; start\n", rtn_name);
    start_trace = true;

}

static void endRTN(char* rtn_name)
{
    fprintf(fp, "%s; end\n", rtn_name);
    start_trace = false;
}

void static getctx(ADDRINT addr, CONTEXT *fromctx, ADDRINT memaddr)

{

	//Only collect traces and a recv function

	if (start_trace == false)

		return;

	fprintf(ref, "%x;%s;%x,%x,%x,%x,%x,%x,%x,%x,%x,\n", addr, opcmap[addr].c_str(),

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


static void Routine(RTN rtn, void *v)
{    

     if(RTN_Valid(rtn)){
     RTN_Open(rtn);
     RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)startRTN, IARG_PTR, RTN_Name(rtn).c_str(), IARG_END);
     RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)endRTN, IARG_PTR, RTN_Name(rtn).c_str(), IARG_END);
     RTN_Close(rtn);
    }
}

static void on_fini(INT32 code, void *v)
{
	fclose(fp);
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



int main(int argc, char *argv[])
{

	if (PIN_Init(argc, argv))
	{
		fprintf(stderr, "command line error\n");
		return 1;
	}
        std::cout << argv[1] << std::endl;
	fp = fopen("function_names.txt", "w");
	PIN_InitSymbols();

	PIN_AddFiniFunction(on_fini, 0);
	RTN_AddInstrumentFunction(Routine, NULL);
	PIN_StartProgram(); // Never returns
	return 0;
}

