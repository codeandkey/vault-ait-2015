#include "vault.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int _test_element(int result, const char* id);

int main(int argc, char** argv)
{
	int output = 0;

	VaultArgList args = vault_args_get(argc, argv);

	printf("Executing on platform %s\n", VAULT_PLATFORM_STRING);
	printf("Starting mode %d, infile = [%s], outfile = [%s], password = [%s]\n", args.mode, args.infile, args.outfile, args.password);

	switch (args.mode) {
	case VAULT_ARG_MODE_HELP:
		printf("Vault client, version %s\n", VAULT_VERSION_STRING);
		printf("Arguments :\n");
		printf("\t-g (--get) <remotefile> <localfile> : Retrieve a file from remote storage.\n");
		printf("\t-p (--put) <localfile> <remotefile> : Upload a file to remote storage.\n");
		printf("\t-l (--list) : List remote files.\n");
		printf("\t-e (--encrypt) <infile> <outfile> : Encrypt file locally with globalkey.\n");
		printf("\t-d (--decrypt) <infile> <outfile> : Decrypt file locally with globalkey.\n");
		printf("\t-p (--password) <password> : Use additional encrpytion password.\n");
		printf("\t-s (--share) <remotefile> <group> : Add a file to a sharing group.\n");
		printf("\t-r (--revoke) <remotefile> <group> : Revoke a file from a sharing group (the group may have already downloaded the file!)\n");
		printf("\t-c (--config) : Run first-time configuration.\n");
		printf("\t-u (--configshare) : Run sharing group configuration.\n");
		printf("\t-h (--help) : Show this message.\n");
		break;
	case VAULT_ARG_MODE_CONFIG:
		output = _test_element(vault_wiz(), "configuration");
		break;
	case VAULT_ARG_MODE_ENCRYPT:
		{
			char* key = vault_file_read("~/.vault_key");
			output = _test_element(vault_encrypt_aes_file(args.infile, args.outfile, key, strlen(key)), "globalkey encryption");
			free(key);
		}
		break;
	case VAULT_ARG_MODE_DECRYPT:
		{
			char* key = vault_file_read("~/.vault_key");
			output = _test_element(vault_decrypt_aes_file(args.infile, args.outfile, key, strlen(key)), "globalkey decryption");
			free(key);
		}
		break;
	case VAULT_ARG_MODE_GET:
		break;
	case VAULT_ARG_MODE_PUT:
		break;
	case VAULT_ARG_MODE_LIST:
		break;
	case VAULT_ARG_MODE_SHARE:
		break;
	case VAULT_ARG_MODE_CONFIGSHARE:
		break;
	case VAULT_ARG_MODE_REVOKE:
		break;
	case VAULT_ARG_MODE_INVALID:
		printf("Invalid arguments! See help with --help.\n");
		break;
	}

	return output;
}

int _test_element(int result, const char* id)
{
	if (!result) {
		printf("[%s] Failed.\n", id);
		return -1;
	} else {
		printf("[%s] Succeeded.\n", id);
		return 0;
	}
}
