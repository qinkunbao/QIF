#!/bin/sh

#How to build mbedTLS for 32-bit programs, with shared objects

#Download a release version of mbedtls
git clone https://github.com/ARMmbed/mbedtls.git --branch mbedtls-2.9.0
cd mbedtls
git checkout -b mbedtls-2.9.0

# build mbedtls
mkdir build
cd build
export CFLAGS=-m32
cmake -DUSE_SHARED_MBEDTLS_LIBRARY=On ..
make
cd ../..




