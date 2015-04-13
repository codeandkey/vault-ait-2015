#include "vault.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define VAULT_MODE_UNDEF -1

/* VAULT_MODE_CONFIG : generate the global user key */
#define VAULT_MODE_CONFIG 0

/* VAULT_MODE_STORE : encrypt a file with the global user key and upload it */
#define VAULT_MODE_STORE 1

/* VAULT_MODE_GET : retrieve a file and decrypt it with the global user key */
#define VAULT_MODE_GET 2

/* VAULT_MODE_DEC : decrypt a file with the global user key */
#define VAULT_MODE_DEC 3

/* VAULT_MODE_ENC : encrypt a file with the global user key */
#define VAULT_MODE_ENC 4

/* VAULT_MODE_LS : list remote files */
#define VAULT_MODE_LS 5

void check_format(int argc, char** argv, int key, int expected_args);
int encrypt_file(char* ifile, char* ofile, char* key, int mode_dec);
int encrypt_file_inplace(char* file, char* key, int mode_dec);

int main(int argc, char** argv) {
	vault_print(VAULT_DBG, "Starting Vault..");

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
		vault_print(VAULT_CRT, "vault: no mode specified");
		return 1;
	}

	vault_print(VAULT_DBG, "key = [%s], localfile = [%s], remotefile = [%s]", enc_key, local_file, remote_file);

	if (exec_mode == VAULT_MODE_CONFIG) {
		vault_print(VAULT_DBG, "vault: starting config mode");

		if (vault_conf_prompt()) {
			vault_print(VAULT_DBG, "vault: config generation reported success\n");
		} else {
			vault_print(VAULT_DBG, "vault: config generation reported failure\n");
		}

		return 0;
	}

	vault_print(VAULT_DBG, "vault: reading configuration..\n");

	char globalkey[1024] = {0};

	FILE* vault_conf = fopen("vault.conf", "r");

	if (!vault_conf) {
		printf("vault: failed to open configuration file. run with --config to generate it.\n");
		return 1;
	}

	fgets(globalkey, 1024, vault_conf);
	globalkey[strlen(globalkey) - 1] = 0;

	fclose(vault_conf);

	switch (exec_mode) {
	case VAULT_MODE_ENC:
		/* We are simply encrypting a file with the global key. */
		{
			if (!encrypt_file_inplace(local_file, globalkey, 0)) {
				vault_print(VAULT_DBG, "vault: encryption failed");
			} else {
				vault_print(VAULT_DBG, "vault: encryption succeeded");
			}
		}
		break;
	case VAULT_MODE_DEC:
		{
			if (!encrypt_file_inplace(local_file, globalkey, 1)) {
				vault_print(VAULT_DBG, "vault: decryption failed");
			} else {
				vault_print(VAULT_DBG, "vault: decryption succeeded");
			}
		}
		break;
	}

	return 0;
}

void check_format(int argc, char** argv, int i, int expected_args) {
	for (int tmp = i + 1; tmp <= i + expected_args; tmp++) {
		if (tmp >= argc || (argv[tmp][0] == '-' && argv[tmp][1] == '-')) {
			vault_print(VAULT_CRT, "option [%s] expects %d argument%s", argv[i], expected_args, expected_args == 1 ? "" : "s");
			return;
		}
	}
}

int encrypt_file(char* ifilename, char* ofilename, char* key, int mode_dec) {
	FILE* ifile = fopen(ifilename, "rb"), *ofile = fopen(ofilename, "wb");

	if (!ifile || !ofile) {
		vault_print(VAULT_CRT, "encrypt_file: failed to open files\n");
		return 0;
	}

	fseek(ifile, 0, SEEK_END);
	int ifile_size = ftell(ifile);
	fseek(ifile, 0, SEEK_SET);

	printf("encrypt_file: read file size as %d\n", ifile_size);

	char* ifile_buffer = malloc(ifile_size + 1);

	ifile_buffer[ifile_size] = 0;

	fread(ifile_buffer, 1, ifile_size, ifile);

	if (ifile_buffer[ifile_size - 1] == '\n') {
		ifile_buffer[ifile_size-- - 1] = 0;
	}

	fclose(ifile);

	if (!mode_dec) {
		vault_encrypt_aes256(ifile_buffer, key);
	} else {
		vault_decrypt_aes256(ifile_buffer, key);
	}

	fwrite(ifile_buffer, 1, ifile_size, ofile);
	fclose(ofile);

	return 1;
}

int encrypt_file_inplace(char* filename, char* key, int mode_dec) {
	char* newstr = malloc(strlen(filename) + 2);
	newstr[strlen(filename) + 1] = 0;
	newstr[0] = '.';
	strcpy(newstr + 1, filename);

	if (!encrypt_file(filename, newstr, key, mode_dec)) {
		return 0;
	}

	char cmd[1024] = {0};
	snprintf(cmd, 1024, "mv %s %s", newstr, filename);

	system(cmd);

	return 1;
}
