#include "sym_crypt.h"
#include "file.h"
#include "hash.h"
#include "util.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <gcrypt.h>

static int init = 0;

static int _vault_gcrypt_check_init(void);
static char* _vault_crypt_getctr(void);

char* vault_encrypt_aes(char* buffer, int buffer_size, char* key, int key_size)
{
	if (!_vault_gcrypt_check_init()) {
		return NULL;
	}

	if (key_size != 32) {
		return NULL;
	}

	/* For the IV, we want to generate a unique one and store it in the new buffer. */

	gcry_cipher_hd_t hd;
	gcry_error_t err;

	err = gcry_cipher_open(&hd, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CTR, 0);

	if (err) {
		printf("Failed to open gcrypt cipher.\n");
		return NULL;
	}

	err = gcry_cipher_setkey(hd, key, key_size);

	if (err) {
		printf("Failed to set gcrypt cipher key.\n");
		return NULL;
	}

	char* ctr = _vault_crypt_getctr();
	err = gcry_cipher_setctr(hd, (void*) ctr, 16);

	if (err) {
		printf("Failed to set gcrypt cipher counter. %s\n", gcry_strerror(err));
		return NULL;
	}

	/* The encrypted data format has the header with the SHA-256 hash of the ciphertext and the CTR value. TODO : Implement a HMAC-SHA256 mode for more security. */

	char* output_buffer = malloc(64 + buffer_size + 1);

	if (!output_buffer) {
		return NULL;
	}

	memset(output_buffer, 0, 64 + buffer_size + 1);

	err = gcry_cipher_encrypt(hd, output_buffer + 64, buffer_size, buffer, buffer_size);

	if (err) {
		printf("Failed to encrypt data.\n");
		return NULL;
	}

	vault_hash(output_buffer + 64, buffer_size, output_buffer, "_vaultsalt");
	memcpy(output_buffer + 32, ctr, 32);

	printf("Encrypted using AES. original buffer size = %d, output buffer size = %d\n", buffer_size, buffer_size + 64);
	printf("KEY : ");
	vault_util_printhex(key, 32);

	printf("\nCTR : ");
	vault_util_printhex(output_buffer + 32, 32);

	printf("\nSHA : ");
	vault_util_printhex(output_buffer, 32);

	printf("\nPTX : ");
	vault_util_printhex(buffer, buffer_size);

	printf("\nCTX : ");
	vault_util_printhex(output_buffer + 64, buffer_size);

	printf("\n");

	gcry_cipher_close(hd);
	free(ctr);

	return output_buffer;
}

char* vault_decrypt_aes(char* buffer, int buffer_size, char* key, int key_size)
{
	if (!_vault_gcrypt_check_init()) {
		return NULL;
	}

	if (key_size != 32) {
		return NULL;
	}

	gcry_cipher_hd_t hd;
	gcry_error_t err;

	err = gcry_cipher_open(&hd, GCRY_CIPHER_AES256, GCRY_CIPHER_MODE_CTR, 0);

	if (err) {
		printf("Failed to open gcrypt cipher.\n");
		return NULL;
	}

	err = gcry_cipher_setkey(hd, key, key_size);

	if (err) {
		printf("Failed to set gcrypt cipher key.\n");
		return NULL;
	}

	/* First, verify the hash. */

	char hash[33];
	hash[32] = 0;

	vault_hash(buffer + 64, buffer_size - 64, hash, "_vaultsalt");

	printf("Decrypting using AES.. buffer size = %d\n", buffer_size);
	printf("KEY : ");
	vault_util_printhex(key, 32);

	printf("\nCTR : ");
	vault_util_printhex(buffer + 32, 32);

	printf("\nSHA (stored) : ");
	vault_util_printhex(buffer, 32);

	printf("\nSHA (local)  : ");
	vault_util_printhex(hash, 32);

	printf("\nCTX  : ");
	vault_util_printhex(buffer + 64, buffer_size - 64);

	printf("\n");

	for (int i = 0; i < 32; i++) {
		if (hash[i] != buffer[i]) {
			printf("Integrity verification failed. An attacker may be trying to do something fishy! [index = %d, calc. hash value = %02X, stored hash value = %02X]\n", i, (unsigned) hash[i] & 0xFF, (unsigned) buffer[i] & 0xFF);
			return NULL;
		}
	}

	err = gcry_cipher_setctr(hd, (void*) (buffer + 32), 16);

	if (err) {
		printf("Failed to set gcrypt counter.\n");
		return NULL;
	}

	/* The encrypted data format has the header with the SHA-256 hash of the ciphertext and the CTR value. TODO : Implement a HMAC-SHA256 mode for more security. */

	char* output_buffer = malloc(buffer_size - 64 + 1);

	if (!output_buffer) {
		return NULL;
	}

	output_buffer[buffer_size - 64] = 0;

	err = gcry_cipher_decrypt(hd, output_buffer, buffer_size - 64,  buffer + 64, buffer_size - 64);

	if (err) {
		printf("Failed to decrypt data.\n");
		return NULL;
	}

	gcry_cipher_close(hd);

	printf("PTX  : ");
	vault_util_printhex(output_buffer, buffer_size - 64);

	printf("\n");

	return output_buffer;
}

int vault_encrypt_aes_file(char* infile, char* outfile, char* key, int key_size)
{
	vault_buffer file_buf = vault_file_read(infile);

	if (!file_buf.ptr) {
		return 0;
	}

	char* enc_data = vault_encrypt_aes(file_buf.ptr, file_buf.size, key, key_size);

	if (!enc_data) {
		return 0;
	}

	FILE* fd = fopen(outfile, "w");

	if (!fd) {
		return 0;
	}

	printf("File buffer size is %d, writing %d bytes to output file.\n", file_buf.size, file_buf.size + 64);

	fwrite(enc_data, 1, file_buf.size + 64, fd);
	fclose(fd);

	free(file_buf.ptr);
	free(enc_data);

	return 1;
}

int vault_decrypt_aes_file(char* infile, char* outfile, char* key, int key_size)
{
	vault_buffer file_buf = vault_file_read(infile);

	if (!file_buf.ptr) {
		return 0;
	}

	char* enc_data = vault_decrypt_aes(file_buf.ptr, file_buf.size, key, key_size);

	if (!enc_data) {
		return 0;
	}

	FILE* fd = fopen(outfile, "w");

	if (!fd) {
		return 0;
	}

	printf("File buffer size is %d, writing %d bytes to output file.\n", file_buf.size, file_buf.size - 64);

	fwrite(enc_data, 1, file_buf.size - 64, fd);
	fclose(fd);

	free(file_buf.ptr);
	free(enc_data);

	return 1;
}

char* _vault_crypt_getctr(void)
{
	/* We simply want to generate a good nonce. We can do this by pulling from /dev/urandom (does not need to be cryptographically secure. */
	/* since the block size is 32 bytes, we pull that much data. */

#if defined(VAULT_PLATFORM_WIN32) || defined(VAULT_PLATFORM_OSX)
	printf("Nonce generation has not been implemented yet for this platform!\n");
	return "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
#endif

	FILE* dev_rand = fopen("/dev/urandom", "r");

	if (!dev_rand) {
		printf("Failed to open /dev/urandom for nonce generation.\n");
		return NULL;
	}

	char* output_buffer = malloc(32);

	fread(output_buffer, 1, 32, dev_rand);
	fclose(dev_rand);

	return output_buffer;
}

int _vault_gcrypt_check_init(void)
{
	if (!init) {
		if (!gcry_check_version(GCRYPT_VERSION)) {
			printf("libgcrypt version mismatch!\n");
			return 0;
		}

		gcry_control(GCRYCTL_DISABLE_SECMEM, 0);
		gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);

		init = 1;
	}

	return 1;
}
