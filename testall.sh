#!/bin/sh

# Test all mbed TLS references
../test_vs_traces.pl  AES ../references/mbedtls/aes_128_cbc/ ../testVectors/traces/
../test_vs_traces.pl  AES ../references/mbedtls/aes_256_cbc/ ../testVectors/traces/
../test_vs_traces.pl  DES ../references/mbedtls/des_ecb/ ../testVectors/traces/
../test_vs_traces.pl  MD5 ../references/mbedtls/md5/ ../testVectors/traces/
../test_vs_traces.pl  RC4 ../references/mbedtls/rc4/ ../testVectors/traces/
../test_vs_traces.pl  SHA1 ../references/mbedtls/SHA1/ ../testVectors/traces/
../test_vs_traces.pl  SHA256 ../references/mbedtls/SHA256/ ../testVectors/traces/
../test_vs_traces.pl  SHA512 ../references/mbedtls/SHA512/ ../testVectors/traces/

# Test all openssl references
../test_vs_traces.pl  AES ../references/openssl/aes/ ../testVectors/traces/
../test_vs_traces.pl  DES ../references/openssl/des/ ../testVectors/traces/
../test_vs_traces.pl  MD5 ../references/openssl/md5/ ../testVectors/traces/
../test_vs_traces.pl  RC4 ../references/openssl/rc4/ ../testVectors/traces/

# Test all 'others' references
aes2  aes3  md5a  rc4a  rc4b  sha1b  sha1c  sha256a  sha256b  sha512a  sha512b
../test_vs_traces.pl  AES ../references/others/aes2/ ../testVectors/traces/
../test_vs_traces.pl  AES ../references/others/aes3/ ../testVectors/traces/
../test_vs_traces.pl  MD5 ../references/others/md5a/ ../testVectors/traces/
../test_vs_traces.pl  RC4 ../references/others/rc4a/ ../testVectors/traces/
../test_vs_traces.pl  SHA1 ../references/others/sha1b/ ../testVectors/traces/
../test_vs_traces.pl  SHA1 ../references/others/sha1c/ ../testVectors/traces/
../test_vs_traces.pl  SHA256 ../references/others/sha256a/ ../testVectors/traces/
../test_vs_traces.pl  SHA256 ../references/others/sha256b/ ../testVectors/traces/
../test_vs_traces.pl  SHA512 ../references/others/sha512a/ ../testVectors/traces/
../test_vs_traces.pl  SHA512 ../references/others/sha512b/ ../testVectors/traces/

