AllTracer
Log every instructions during runtime

MainExecutableTracer
Log every instructions in the main MainExecutableTracer

SharedLibraryDetection
Log every dynamic linking libraries
e.g.
Loading module /home/qinkun/new/malwarecrypto/target/openssl/md5
Module Base: 08048000
Module end: 08048747
Loading module /lib/ld-linux.so.2
Module Base: b7f2a000
Module end: b7f4c043
Loading module [vdso]
Module Base: b7f28000
Module end: b7f28d34
Loading module /lib/i386-linux-gnu/libcrypto.so.1.0.0
Module Base: b54f6000
Module end: b56e2677
Loading module /lib/i386-linux-gnu/libc.so.6
Module Base: b5330000
Module end: b54e5a1b
Loading module /lib/i386-linux-gnu/libdl.so.2
Module Base: b532b000
Module end: b532f067


DynamicProcedures
Log every procedures 
e.g.
Loading module /lib/i386-linux-gnu/libcrypto.so.1.0.0
Procedure CRYPTO_get_dynlock_create_callback
Loading module /lib/i386-linux-gnu/libcrypto.so.1.0.0
Procedure CRYPTO_get_dynlock_lock_callback
Loading module /lib/i386-linux-gnu/libcrypto.so.1.0.0
Procedure CRYPTO_get_dynlock_destroy_callback



Build
$ make TARGET=ia32 PIN_ROOT=<path to pin kit>

Run
$ <path to pin kit>/pin -t obj-ia32/MyPinTool.so -- <your program>
