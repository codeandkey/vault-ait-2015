#include "vault_rsa.h"
#include "vault_cli.h"

#include <stdlib.h>
#include <stdio.h>

int vault_rsa_genkeys(void) {
	vault_print(VAULT_DBG, "Generating RSA keys..");

	int result = system("openssl genrsa -out .vault_private.pem 4096");
	result &= system("openssl rsa -pubout -in .vault_private.pem > .vault_public.pem");

	return result;
}

int vault_rsa_encrypt(char* infile, char* outfile, char* inkey) {
	char command[2048] = {0};

	 snprintf(command, 2048, "openssl rsautl -in %s -out %s -encrypt -inkey %s", infile, outfile, inkey);

	return system(command);
}

int vault_rsa_decrypt(char* infile, char* outfile) {
	char command[2048] = {0};

	snprintf(command, 2048, "openssl rsautl -in %s -out %s -decrypt -inkey .vault_private.pem", infile, outfile);

	return system(command);
}

int vault_rsa_sign(char* infile, char* out_sigfile) {
	char command[2048] = {0};

	snprintf(command, 2048, "openssl dgst -sha256 -sign .vault_private.pem -out %s %s", out_sigfile, infile);

	return system(command);
}

int vault_rsa_verify(char* infile, char* in_sigfile, char* inkey) {
	char command[2048] = {0};

	snprintf(command, 2048, "openssl dgst -sha256 -verify %s -signature %s %s", inkey, in_sigfile, infile);

	return system(command);
}
