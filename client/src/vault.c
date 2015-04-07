#include "vault.h"

#include <stdlib.h>
#include <string.h>

#define VAULT_MODE_UNDEF -1
#define VAULT_MODE_CONFIG 0
#define VAULT_MODE_STORE 1
#define VAULT_MODE_GET 2

void check_format(int argc, char** argv, int key, int expected_args);

int main(int argc, char** argv) {
	vault_print(VAULT_DBG, "Starting Vault..");

	int exec_mode = VAULT_MODE_UNDEF;
	int enc_mode = 0;

	char* local_file = NULL;
	char* remote_file = NULL;
	char* enc_key = NULL;

	for (int i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "--enc")) {
			check_format(argc, argv, i, 1);

			enc_mode = 1;
			enc_key = argv[i + 1];
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
	}

	vault_print(VAULT_DBG, "key = [%s], localfile = [%s], remotefile = [%s]", enc_key, local_file, remote_file);

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
