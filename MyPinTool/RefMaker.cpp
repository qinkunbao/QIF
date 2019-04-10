/*
* A pin tool to record the trace of a specific function 
*
*/
#include <string>
#include <stdio.h>
#include <fstream>
#include <pin.H>
#include <map>
#include <iostream>
#include <vector>


std::map<ADDRINT, string> opcmap;
FILE *fp, *ref;
bool start_trace = false;
std::string function_name;
int output_count = 0;           /* Output the trace of the specificed function once */
int max_count = 1;

static void startRTN(char* rtn_name)
{
    fprintf(fp, "Entering %s", rtn_name);
    if(output_count < max_count) {
        start_trace = true;
        fprintf(fp, ", starting trace");
    }
    fprintf(fp, "\n");
}

static void endRTN(char* rtn_name)
{
    fprintf(fp, "Leaving %s\n", rtn_name);
    if(start_trace) {
        output_count++;
    }
    start_trace = false;
}

void static getctx(ADDRINT addr, CONTEXT *fromctx, ADDRINT memaddr)
{
    if(!start_trace)
        return;

    fprintf(fp, "%x;%s;%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,\n", addr, opcmap[addr].c_str(),
            PIN_GetContextReg(fromctx, REG_EAX),
            PIN_GetContextReg(fromctx, REG_EBX),
            PIN_GetContextReg(fromctx, REG_ECX),
            PIN_GetContextReg(fromctx, REG_EDX),
            PIN_GetContextReg(fromctx, REG_ESI),
            PIN_GetContextReg(fromctx, REG_EDI),
            PIN_GetContextReg(fromctx, REG_ESP),
            PIN_GetContextReg(fromctx, REG_EBP),
            memaddr,
            PIN_GetContextReg(fromctx, REG_EFLAGS));
}


static void Routine(RTN rtn, void *v)
{    
     if(RTN_Name(rtn).find(function_name) == std::string::npos)
        return;
     if(RTN_Name(rtn).find("@plt") != std::string::npos)
        return;
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
    if (PIN_Init(argc, argv)) {      
        fprintf(stderr, "command line error\n");
        std::cout << "./pin -t RefMaker.so -- target function_name" << std::endl;
        return 1;
    }

    /* The arguments to the pin tool vary from one platform to another, but we
     * do know that the last argument will be the function name */
    /* TODO: The correct way to pass options to a pin tool is document here:
     * https://software.intel.com/sites/landingpage/pintool/docs/97619/Pin/html/index.html#BUILDINGTOOLS
     * How we're doing it now kind a works, but `function_name` is also getting
     * passed to `target`.  Fortunately our target programs ignore argv[1] 
     */
    function_name = argv[argc-1];
    std::cout << "Instrument Function: " << function_name <<std::endl;

    /* The sequence of calls to the chosen function can be logged to a file,
     * but we don't use that file so we just log to stdout */
//    string log_file = function_name + "_calls.log";
//    fp = fopen(log_file.c_str(), "w");
    fp = stdout;

    string trace_file_name = function_name;
    ref = fopen(trace_file_name.c_str(), "w");
    PIN_InitSymbols();

    PIN_AddFiniFunction(on_fini, 0);
    RTN_AddInstrumentFunction(Routine, NULL);
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_StartProgram(); // Never returns
    return 0;
}

