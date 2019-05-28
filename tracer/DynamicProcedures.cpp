/*
* A pin tool to record all routines except ntdll.dll
*
*/

#include <stdio.h>
#include <pin.H>
#include <map>
#include <iostream>

const char *tracefile = "dynamic_instrace.txt";
std::map<ADDRINT, string> opcmap;
FILE *fp;

static void Routine(char* rtn_name, char* module_name)
{
	fprintf(fp, "Loading module %s  ", module_name);
	fprintf(fp, "Routine name %s \n", rtn_name);

}

static void images(IMG img, void *v)
{

	for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
	{
		for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
		{
			RTN_Open(rtn);
			if (RTN_Name(rtn).find("Rtl")) { //Try to filter out "Rtl"
				RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)Routine, IARG_PTR, RTN_Name(rtn).c_str(), IARG_PTR, IMG_Name(img).c_str(), IARG_END);
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

