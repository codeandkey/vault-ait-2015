#include "hash.h"
#include "util.h"

#include <openssl/sha.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void vault_hash(char* input, int input_size, char* buffer32, char* salt)
{
	int salt_size = salt ? strlen(salt) : 0;

	char* input_buffer = malloc(salt_size + input_size + 1);
	input_buffer[salt_size + input_size] = 0;

	if (!input_buffer) {
		printf("Hash malloc failed!\n");
		return;
	}

	if (salt_size) {
		memcpy(input_buffer, salt, salt_size);
	}

	memcpy(input_buffer + salt_size, input, input_size);
	SHA256((unsigned char*) input_buffer, input_size + salt_size, (unsigned char*) buffer32);

	free(input_buffer);
}
