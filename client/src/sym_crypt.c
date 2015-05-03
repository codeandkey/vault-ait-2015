#include "sym_crypt.h"

char* vault_crypt_aes(char* buffer, int buffer_size, char* key, int key_size, int encrypt)
{
	if (key_size != 32) {
		return NULL;
	}

	/* For the IV, we want to generate a unique one and store it in the new buffer. */

	gcry_cipher_hd_t hd;
	gcry_cipher_open(&hd, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_GCM, 0);
	gcry_cipher_setkey(hd, key, key_size);
	gcry_cipher_setiv(hd, iv, iv_size);
}
