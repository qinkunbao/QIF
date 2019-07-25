# Trace Aanlysis

[![Build Status](https://travis-ci.com/qinkunbao/QIF.svg?token=N93BqvToieqFRYyp9sps&branch=master)](https://travis-ci.com/qinkunbao/QIF)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/39bde068886343179a4d82c9cc50e100)](https://www.codacy.com?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=qinkunbao/QIF&amp;utm_campaign=Badge_Grade)

This repo contains five useful tools for trace analysis.
- A Pin Tool
- CryptoHunt
- TaintTrace
- QIF
- Dissambler
- CryptoHunt (Static Version)

The pin tool is used to collect the execution trace. CryptoHunt, TaintTrace and QIF process the trace file and output the corresponding result. 

The dissambler can analyze the ELF/PE file and output the function and the basic blocks. The CryptoHunt (Static Version) can process the result from the Dissambler and output the potential Crypto Function.

## Paper Draft

PDF version (Latest): https://github.com/qinkunbao/QIF/blob/master/papers/SP2020_June_1/main.pdf

QIF-DATA: https://github.com/qinkunbao/QIF-data

QIF Google Document (No update until July 1): 
https://docs.google.com/document/d/1c0Ahiz9vz4jomn_zgYAkRgElKiWANoOlg_HD-i66af4/edit?usp=sharing

Slides:
https://github.com/qinkunbao/QIF/blob/master/slides/Side%20Channel.pdf

## Prerequisites:
- Ensure that you have one of the following systems  
    * Windows 10
    * Ubuntu 16.04 LTS 32bits
    * macOS Mojave
    * Other systems may work but haven't been tested
- Ensure that you have one of the following compilers   
    * Clang 3.3 or newer
    * Microsoft Visual Studio 2017
- Cmake 3.10 or newer 
- Radare2 3.0.1
- R2-pipe

## Build
To Build the Pin Tool, please refer https://software.intel.com/en-us/articles/pin-a-dynamic-binary-instrumentation-tool

To Build CryptoHunt and TaintTrace with CMake
~~~~{.sh}
$ mkdir build
$ cd build
& cmake ..
& make all
~~~~


## Analysis Workflow
1. Collect execution traces for the malware using the Pin Tool. The traces here inlcudes the following information.
    * The memory address of each instruction
    * The Opcode
    * Register Values
    * Source and destination operands
~~~~{.sh}
$ .\pin -t MyPinTools -- <Your programs>
~~~~
Here is an example for the trace file
```
b7fdca90;mov ecx, ebx;6d,1505,bffafb24,80484ff,1,b7ff7000,bffafa58,bffafae0,0,
b7fdca92;add edx, 0x1;6d,1505,1505,80484ff,1,b7ff7000,bffafa58,bffafae0,0,
b7fdca95;shl ecx, 0x5;6d,1505,1505,8048500,1,b7ff7000,bffafa58,bffafae0,0,
b7fdca98;add ebx, ecx;6d,1505,2a0a0,8048500,1,b7ff7000,bffafa58,bffafae0,0,
b7fdca9a;add ebx, eax;6d,2b5a5,2a0a0,8048500,1,b7ff7000,bffafa58,bffafae0,0,
b7fdca9c;movzx eax, byte ptr [edx];6d,2b612,2a0a0,8048500,1,b7ff7000,bffafa58,bffafae0,8048500,
...
```
The pin tool `tracer/AllTracer.cpp` creates traces with the required
information in the correct format.  The repo also contains a PinTool that can
collect traces and split them every time the program reads data from a network
socket.   

2. Run CryptoHunt
~~~~{.sh}
$ cd build
$ ./LoopDetection <The Trace File> // Outputs potential loops
$ python ../start.py <references> <Potential Loops Directory>
~~~~
If any crypto is found, CryptoHunt will output the match information.
Notes
- Currently, CryptoHunt loads the entire trace file to memory, and can
quickly consume all system memory. Trace files larger than 1GB should be
divided into pieces (a 1GB file requires about 4GB of memory, so systems with
4x more memory than the trace file size should be OK)).  
- CryptoHunt output loops in the
current directory, so it's good to create a directory under build, change to
it, then run CryptoHunt.
- If you need to kill all start.py (sub)processes, use `pkill -f ".*start.py.*"
  The `SymbolicExecution` proceses can be killed similarly. `

3. Run TaintAnalysis  
TaintTrace is a small tool to mark those code that actually touches the Network packet. It can be
used to analyze the usage of crypto functions.
- Clone the reposity
- Build the Pin Tool, TaintTrace and two demos in the `examples` directory 
- Run the pin tool to collect execution traces
~~~~{.sh}
$ .\pin -t MyPinTools -- <Your programs>
~~~~
- The Pin Tool will generate two TXT files, `Taint_data.txt` and `Function.txt`.
- Run the taint anaylysis tool
~~~~{.sh}
$ .\TaintTrace Taint_data.txt Function.txt
~~~~
- While both of the two programs call xor_decrypt funcntion, only the program under the `Encrypted` derectory use xor_decrypt to decrypt the traffic packet from the Network.


### Creating References
The directory `taintTrace/references/referenceMaker` contains pin tools used to create references. 
To build them:
~~~~{.sh}
$ cd taintTrace/references/referenceMaker
$ make TARGET=ia32 PIN_ROOT=<path to pin kit>
~~~~
To run them, first build the refrence programs in `target` (use the makefiles in `target/mbedtls`, `target/openssl`, `target/others`.
Once the reference programs are build, run the pin tool with them, for example:
~~~~{.sh}
$ pin -t obj-ia32/FunctionNames.so -- ../../../target/others/md5a
~~~~
This generates a file `function_names.txt` containing all the functions called
by the program.  Then choose a function to generate a reference for, e.g.,
Md5Update, and use the pin tool RefMaker.so, to create a trace of that function.
~~~~{.sh}
pin -t obj-ia32/RefMaker.so -- ../../../target/others/md5a Md5Update
~~~~
The trace is stored in a file with the function name (`Md5Update` in our example). 
Now use CryptoHunt to extract the loops in the trace
~~~~{.sh}
../../build/CryptoHunt Md5Update
~~~~
This will find three loops and output the three files
``
-rw-rw-r-- 1 gregz gregz   554 Aug  1 10:50 Md5Update1.ref
-rw-rw-r-- 1 gregz gregz 10782 Aug  1 10:50 Md5Update2.ref  
-rw-rw-r-- 1 gregz gregz   570 Aug  1 10:50 Md5Update3.ref  
``



### Build Requirements
Visual Studio 2017  
Intel Pin  

### Future Work

- Improve the taint propagation rule
- Test the tool with real malware samples
- Fix existing Bugs (Memory Leakage, Taint Propogation)
- Handle large trace files in CryptoHunt. Probably best to read the trace file
  as a stream, keeping only part of it in memory at a time. 
- Develop heuristics to reduce the number of trivial loops generated from our reference implementations
- Create a tool to ouput the source code corresponding to the ref loop
- Filter the loops output by CryptoHunt, to remove all of those that contain
  instructions (or registers) not present in the target loops.  For example,
  our targets don't contain any vector implementations, so any loops containing
  xmm registers can be removed from the candidate set.
- The loop detection was trying to be aggressive about inlining smaller inner
  loops into outer loops. On one hand this is good; some of the inner loops are
  trivial, and will match many other loops.  Combining them with the outer
  loops reduces the number of loops, and makes the remaining more specific to
  the function we're searching for.  On the other hand sometimes these small
  loops can cause the formulas generated in symbolic execution to grow
  expoenetially, killing performance. For now we will treat the inner loops as
  independent loops, but we should revisit this and see if it can improve
  accuracy to spend more time on loop detection, e.g., we could check the
  formula length of large candidate loops, and only break out inner loops if
  the formulas are too large.  Another approach may be to keep track of which
  inner loops belong to a larger loop, and only consider a match if all of the
  inner loops are matched. 

### Minor issues


