mkdir all
mkdir all/aes
mkdir all/rc4
mkdir all/des
mkdir all/md5
mkdir all/sha1
mkdir all/sha256
mkdir all/sha512
mkdir all/sha384
mkdir ../all/md4
cd mbedtls/
cp aes_128_cbc/* aes_256_cbc/* ../all/aes/
cp aes_256_cbc/mbedtls_aes_crypt_cbc ../all/aes/mbedtls_aes256_crypt_cbc 
cp aes_256_cbc/mbedtls_aes_setkey_enc ../all/aes/mbedtls_aes256_setkey_enc 
cd des_ecb/
cp * ../../all/des/
cd ..
cp md4/* ../all/md4/
cp md5/mbedtls_* ../all/md5/
cd rc4/
mv rc4_crypt/inst.txt rc4_crypt.trace
mv rc4_init/inst.txt rc4_init.trace
cp rc4_*.trace ../../all/rc4/
cd ..
cd sha1
cp * ../../all/sha1/
cd ..
cp sha256/* ../all/sha256/
cp sha384/sha384_wrap ../all/sha384/
cp sha512/* ../all/sha512/
cd ..
cd openssl/
cd no-asm/
cp AES/* ../../all/aes/
cp DES_ECB_128/* ../../all/des/
cp MD5/MD5 ../../all/md5/
cp RC4/* ../../all/rc4/
cp SHA1/SHA1_* ../../all/sha1/
cp SHA256/SHA256 ../../all/sha256/
cp SHA512/SHA512 ../../all/sha512/
cd ..
cd others/
cd aes2/
cp AJ_AES_E* ../../all/aes/
cd ..
cd aes3
cp Aes* ../../all/aes/
cd ..
cp md5a/Md5* ../all/md5/
cp md5b/md5 ../all/md5/md5b
cp rc4a/rc4_* ../all/rc4/
cp rc4b/Rc4* ../all/rc4/
cp sha1b/SH* ../all/sha1/
cp sha1c/Sha1Ctx_* ../all/sha1/
cp sha256a/SHA256_* ../all/sha256/
cp sha256b/Sha256* ../all/sha256/
cp sha512a/SHA512_* ../all/sha512/
cp sha512b/Sha512* ../all/sha512/
cp tinyshell/aes1/aes_* ../all/aes/
cp tinyshell/sha1a/sha1_* ../all/sha1/
