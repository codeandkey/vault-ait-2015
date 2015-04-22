#include "vault_hash.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <openssl/sha.h>

void vault_hash(char* input, char* output, char* salt) {
	unsigned char hash[SHA256_DIGEST_LENGTH] = {0};
	int input_length = strlen(salt) * 2 + strlen(input) + 1;

	unsigned char* hash_input = malloc(input_length);
	hash_input[input_length - 1] = 0;

	strcpy((char*) hash_input, salt);
	strcpy((char*) hash_input + strlen(salt), input);
	strcpy((char*) hash_input + strlen(salt) + strlen(input), salt);

	SHA256(hash_input, strlen((char*) hash_input), hash);

	for (int i = 0; i < 32; i++) {
		sprintf(output + i, "%X", hash[i]);
	}

	free(hash_input);
}
