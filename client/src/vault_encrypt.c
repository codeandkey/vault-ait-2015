#include "vault_encrypt.h"
#include "vault_aes.h"
#include "vault_cli.h"

#include <stdlib.h>
#include <stdio.h>

int vault_crypt_file(char* infile, char* outfile, char* key_hash, int encrypt) {
	FILE* infile_fd = fopen(infile, "rb"), *outfile_fd = NULL;

	int infile_size = 0;

	if (!infile_fd) {
		return 0;
	}

	fseek(infile_fd, 0, SEEK_END);
	infile_size = ftell(infile_fd);
	fseek(infile_fd, 0, SEEK_SET);

	/* Infile is ready to go, we allocate the buffers and read the file data. */

	char* file_buffer = malloc(infile_size + 1);
	fread(file_buffer, 1, infile_size, infile_fd);
	fclose(infile_fd);

	file_buffer[infile_size] = 0;

	/* Encrypt the buffer. */

	int result = 0;

	vault_print(VAULT_DBG, "[vault_crypt_file] encrypt mode = %d, file_size = %d\n", encrypt, infile_size);

	if (encrypt) {
		result = vault_encrypt_aes256(file_buffer, key_hash);
	} else {
		result = vault_decrypt_aes256(file_buffer, key_hash);
	}

	if (!result) {
		return 0;
	}

	outfile_fd = fopen(outfile, "wb");

	if (!outfile_fd) {
		return 0;
	}

	fwrite(file_buffer, 1, infile_size, outfile_fd);
	fclose(outfile_fd);

	free(file_buffer);

	return 1;
}

int vault_encrypt_file(char* infile, char* outfile, char* key_hash) {
	return vault_crypt_file(infile, outfile, key_hash, 1);
}

int vault_decrypt_file(char* infile, char* outfile, char* key_hash) {
	return vault_crypt_file(infile, outfile, key_hash, 0);
}

int vault_encrypt_file_inplace(char* inoutfile, char* key_hash) {
	return vault_encrypt_file(inoutfile, inoutfile, key_hash);
}

int vault_decrypt_file_inplace(char* inoutfile, char* key_hash) {
	return vault_decrypt_file(inoutfile, inoutfile, key_hash);
}
