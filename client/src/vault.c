#include "vault.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define VAULT_MODE_UNDEF -1
#define VAULT_MODE_CONFIG 0 /* VAULT_MODE_CONFIG : generate the global user key */
#define VAULT_MODE_STORE 1 /* VAULT_MODE_STORE : encrypt a file with the global user key and upload it */
#define VAULT_MODE_GET 2 /* VAULT_MODE_GET : retrieve a file and decrypt it with the global user key */
#define VAULT_MODE_DEC 3 /* VAULT_MODE_DEC : decrypt a file with the global user key */
#define VAULT_MODE_ENC 4 /* VAULT_MODE_ENC : encrypt a file with the global user key */
#define VAULT_MODE_LS 5 /* VAULT_MODE_LS : list remote files */
#define VAULT_MODE_SHARE 6 /* VAULT_MODE_SHARE : share file with another user */

void check_format(int argc, char** argv, int key, int expected_args);

int main(int argc, char** argv) {
	vault_print(VAULT_DBG, "Starting Vault..\n");

	int exec_mode = VAULT_MODE_UNDEF;

	char* local_file = NULL;
	char* remote_file = NULL;
	char* enc_key = NULL;

	for (int i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "--enc")) {
			check_format(argc, argv, i, 1);
			enc_key = argv[i + 1];
		}

		if (!strcmp(argv[i], "--config")) {
			exec_mode = VAULT_MODE_CONFIG;
		}

		if (!strcmp(argv[i], "--store")) {
			check_format(argc, argv, i, 1);
			exec_mode = VAULT_MODE_STORE;
			local_file = remote_file = argv[i + 1];
		}

		if (!strcmp(argv[i], "--share")) {
			check_format(argc, argv, i, 2);
			exec_mode = VAULT_MODE_SHARE;
			local_file = argv[i + 1];
			remote_file = argv[i + 2];
		}

		if (!strcmp(argv[i], "--rstore")) {
			check_format(argc, argv, i, 2);
			exec_mode = VAULT_MODE_STORE;

			local_file = argv[i + 1];
			remote_file = argv[i + 2];
		}

		if (!strcmp(argv[i], "--get")) {
			check_format(argc, argv, i, 1);
			exec_mode = VAULT_MODE_GET;

			local_file = remote_file = argv[i + 1];
		}

		if (!strcmp(argv[i], "--rget")) {
			check_format(argc, argv, i, 2);

			exec_mode = VAULT_MODE_GET;

			remote_file = argv[i + 1];
			local_file = argv[i + 2];
		}

		if (!strcmp(argv[i], "--globalenc")) {
			check_format(argc, argv, i, 1);
			exec_mode = VAULT_MODE_ENC;
			local_file = remote_file = argv[i + 1];
		}

		if (!strcmp(argv[i], "--globaldec")) {
			check_format(argc, argv, i, 1);
			exec_mode = VAULT_MODE_DEC;
			local_file = remote_file = argv[i + 1];
		}

		if (!strcmp(argv[i], "--list")) {
			exec_mode = VAULT_MODE_LS;
		}
	}

	if (exec_mode == VAULT_MODE_UNDEF) {
		vault_print(VAULT_CRT, "vault: no mode specified\n");
		return 1;
	}

	char enc_key_hash[33];

	if (enc_key) {
		vault_hash(enc_key, enc_key_hash, "__vault_enc");
		enc_key = enc_key_hash;
		enc_key_hash[32] = 0;
	}

	vault_print(VAULT_DBG, "key = [%s], localfile = [%s], remotefile = [%s]\n", enc_key, local_file, remote_file);

	if (exec_mode == VAULT_MODE_CONFIG) {
		vault_print(VAULT_DBG, "vault: starting config mode\n");

		if (vault_conf_prompt()) {
			vault_print(VAULT_DBG, "vault: config generation reported success\n");
		} else {
			vault_print(VAULT_DBG, "vault: config generation reported failure\n");
		}

		return 0;
	}

	vault_print(VAULT_DBG, "vault: reading global key..\n");

	char globalkey[1024] = {0};

	FILE* vault_conf = fopen("vault.key", "r");

	if (!vault_conf) {
		printf("vault: failed to open key file. run with --config to generate it.\n");
		return 1;
	}

	fgets(globalkey, 1024, vault_conf);
	globalkey[strlen(globalkey) - 1] = 0;

	fclose(vault_conf);

	switch (exec_mode) {
	case VAULT_MODE_ENC:
		/* We are simply encrypting a file with the global key. */
		{
			int result = 1;

			if (enc_key) {
				result &= vault_encrypt_file_inplace(local_file, enc_key);
			}

			if (result) {
				result &= vault_encrypt_file_inplace(local_file, globalkey);
			}

			if (!result) {
				vault_print(VAULT_CRT, "vault: encryption failed");
			}
		}
		break;
	case VAULT_MODE_DEC:
		{
			int result = 1;

			if (enc_key) {
				result &= vault_decrypt_file_inplace(local_file, enc_key);
			}

			if (result) {
				result &= vault_decrypt_file_inplace(local_file, globalkey);
			}

			if (!result) {
				vault_print(VAULT_CRT, "vault: decryption failed");
			}
		}
		break;
	case VAULT_MODE_SHARE:
		{
			int result = vault_share(local_file, remote_file);

			if (!result) {
				vault_print(VAULT_CRT, "vault: sharing failed");
			}
		}
		break;
	}

	return 0;
}

void check_format(int argc, char** argv, int i, int expected_args) {
	for (int tmp = i + 1; tmp <= i + expected_args; tmp++) {
		if (tmp >= argc || (argv[tmp][0] == '-' && argv[tmp][1] == '-')) {
			vault_print(VAULT_CRT, "option [%s] expects %d argument%s\n", argv[i], expected_args, expected_args == 1 ? "" : "s");
			return;
		}
	}
}
