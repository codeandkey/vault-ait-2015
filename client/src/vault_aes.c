#include "vault_aes.h"

#include <stdlib.h>
#include <string.h>

#include <gcrypt.h>
#include <assert.h>

static int initialized = 0;

static void check_init(void);
static int vault_aes256(char* buffer, char* key, char* iv, int enc_mode);

int vault_encrypt_aes256(char* buffer, char* key) {
	return vault_aes256(buffer, key, "1234567890123456", 1);
}

int vault_decrypt_aes256(char* buffer, char* key) {
	return vault_aes256(buffer, key, "1234567890123456", 0);
}

void check_init(void) {
	if (!initialized) {
		initialized = 1;
		assert(gcry_check_version(GCRYPT_VERSION));

		gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
	}
}

int vault_aes256(char* buffer, char* key, char* ini_vector, int enc_mode) {
	check_init();

	size_t key_length = gcry_cipher_get_algo_keylen(GCRY_CIPHER_AES256);
	size_t block_length = gcry_cipher_get_algo_blklen(GCRY_CIPHER_AES256);
	size_t ini_length = strlen(ini_vector);
	size_t text_length = strlen(buffer);

	printf("vault_aes256: given text: [%s]\n", buffer);

	printf("vault_aes256: AES256 key length = %zu, block length = %zu, given key length = %zu, given IV length = %zu, given text length = %zu\n", key_length, block_length, strlen(key), ini_length, text_length);

	if (ini_length != block_length) {
		printf("vault_aes256: ini length [%zu] must be equal to the block length [%zu]\n", ini_length, block_length);
		return 0;
	}

	if (strlen(key) != key_length) {
		printf("vault_aes256: key length [%zu] must be equal to the algorithm key length [%zu]\n", strlen(key), key_length);
		return 0;
	}

	char* output_buffer = malloc(text_length + 1);

	if (!output_buffer) {
		return 0;
	}

	gcry_error_t err;
	gcry_cipher_hd_t hd;

	err = gcry_cipher_open(&hd, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CTR, 0);

	if (err) {
		printf("vault_aes256: failed to open cipher [%s]\n", gcry_strerror(err));
		return 0;
	}

	err = gcry_cipher_setkey(hd, key, key_length);

	if (err) {
		printf("vault_aes256: failed to set key [%s]\n", gcry_strerror(err));
		return 0;
	}

	err = gcry_cipher_setiv(hd, ini_vector, block_length);

	if (err) {
		printf("vault_aes256: failed to set iv [%s]\n", gcry_strerror(err));
		return 0;
	}

	if (enc_mode) {
		err = gcry_cipher_encrypt(hd, buffer, text_length, NULL, 0);
	} else {
		err = gcry_cipher_decrypt(hd, buffer, text_length, NULL, 0);
	}

	if (err) {
		printf("vault_aes256: failed to de/encrypt [%s]\n", gcry_strerror(err));
		return 0;
	}

	gcry_cipher_close(hd);

	return 1;
}
