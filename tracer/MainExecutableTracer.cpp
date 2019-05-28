/*
 * A pin tool to record all instructions in a binary execution.
 *
 */

#include <stdio.h>
#include <pin.H>
#include <map>
#include <iostream>

const char *tracefile = "main_instrace.txt";
std::map<ADDRINT, string> opcmap;
FILE *fp;

void getctx(ADDRINT addr, CONTEXT *fromctx, ADDRINT memaddr)
{
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

static void images(IMG img, void *v)
{
        if (!IMG_IsMainExecutable(img))
                return;

        for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
        {
                for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
                {
                        RTN_Open(rtn);
                        for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins))
                        {
                                ADDRINT addr = INS_Address(ins);
                                if (opcmap.find(addr) == opcmap.end())
                                {
                                        opcmap.insert(std::pair<ADDRINT, string>(addr, INS_Disassemble(ins)));
                                }

                                if (INS_IsMemoryRead(ins))
                                {
                                        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)getctx, IARG_INST_PTR, IARG_CONST_CONTEXT, IARG_MEMORYREAD_EA, IARG_END);
                                }
                                else if (INS_IsMemoryWrite(ins))
                                {
                                        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)getctx, IARG_INST_PTR, IARG_CONST_CONTEXT, IARG_MEMORYWRITE_EA, IARG_END);
                                }
                                else
                                {
                                        INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)getctx, IARG_INST_PTR, IARG_CONST_CONTEXT, IARG_ADDRINT, 0, IARG_END);
                                }
                        }
                        RTN_Close(rtn);
                }
        }
}

static void on_fini(INT32 code, void *v)
{
        fclose(fp);
}

int main(int argc, char *argv[])
{

        if (PIN_Init(argc, argv))
        {
                fprintf(stderr, "command line error\n");
                return 1;
        }

        fp = fopen(tracefile, "w");

        PIN_InitSymbols();

        PIN_AddFiniFunction(on_fini, 0);
        IMG_AddInstrumentFunction(images, NULL);
        //INS_AddInstrumentFunction(instruction, NULL);

        PIN_StartProgram(); // Never returns
        return 0;
}
