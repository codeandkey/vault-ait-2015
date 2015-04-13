#include "vault_aes.h"

#include <stdlib.h>
#include <string.h>

#include <gcrypt.h>

void vault_encrypt_aes256(char* buffer, char* key) {
	gcry_cipher_hd_t hd = NULL;
	gcry_error_t err;

	err = gcry_cipher_open(&hd, GCRY_CIPHER_AES256, CGRY_CIPHER_MODE_CBC, GCRY_CIPHER_SECURE);

	if (err !=

}

void vault_decrypt_aes256(char* buffer, char* key) {
}
