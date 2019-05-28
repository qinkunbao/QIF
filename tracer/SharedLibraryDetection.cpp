#include <stdio.h>
#include <pin.H>
#include <map>
#include <iostream>

const char *tracefile = "libraries.txt";
std::map<ADDRINT, string> opcmap;
FILE *fp;

 VOID ImageLoad(IMG Img, VOID *v)  
 {  
     fprintf(fp, "Loading module %s \n", IMG_Name(Img).c_str());  
     fprintf(fp, "Module Base: %08x \n", IMG_LowAddress(Img));  
     fprintf(fp, "Module end: %08x \n", IMG_HighAddress(Img));  
     fflush(fp);  
 }


static void on_fini(INT32 code, void *v)
{
     fclose(fp);
}

int main(int argc, char *argv[])
{

     if (PIN_Init(argc, argv)) {
          fprintf(stderr, "command line error\n");
          return 1;
     }

     fp = fopen(tracefile, "w");

     PIN_InitSymbols();

     PIN_AddFiniFunction(on_fini, 0);
     IMG_AddInstrumentFunction(ImageLoad, 0); 

     PIN_StartProgram(); // Never returns
     return 0;
}
