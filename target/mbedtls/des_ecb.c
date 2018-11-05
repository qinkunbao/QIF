#include <stdio.h>
#include <string.h>
#include <mbedtls/des.h>

static const unsigned char key[MBEDTLS_DES_KEY_SIZE] = { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 };

#ifdef DEBUG
void print_hex(const char* s, uint8_t* data, size_t len)
{
    printf("%s: ", s);
    for (size_t i = 0; i < len; i++) {
        printf("%02X", data[i]);
    }
    printf("\n");
}
#endif

int main()
{
    unsigned char plaintext[8];
    unsigned char ciphertext[8];
    unsigned char dec_out[8];
	mbedtls_des_context ctx;

	mbedtls_des_init(&ctx);

	memset(plaintext, 0x07, sizeof(plaintext));

	int ret = mbedtls_des_setkey_enc(&ctx, key);
	if(ret != 0) {
		printf("Failed to set DES key for encrpytion\n");
		return -1;
	}

    ret = mbedtls_des_crypt_ecb(&ctx, plaintext, ciphertext);
	if(ret != 0) {
		printf("Failed to DES-ECB encrypt\n");
		return -1;
	}

	ret = mbedtls_des_setkey_dec(&ctx, key);
	if(ret != 0) {
		printf("Failed to set DES key for decryption\n");
		return -1;
	}

    ret = mbedtls_des_crypt_ecb(&ctx, ciphertext, dec_out);
	if(ret != 0) {
		printf("Failed to DES-ECB decrypt\n");
		return -1;
	}

#ifdef DEBUG
	print_hex("plaintext", plaintext, sizeof(plaintext));
	print_hex("ciphertext", ciphertext, sizeof(ciphertext));
	print_hex("decrypted", dec_out, sizeof(dec_out));
#endif

	mbedtls_des_free(&ctx);
    return 0;
} 
