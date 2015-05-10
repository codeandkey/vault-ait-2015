#include "arg.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static int _vault_args_check_format(int argc, char** argv, int i, int arg_count);

vault_arg_list vault_args_get(int argc, char** argv)
{
	vault_arg_list output;
	memset(&output, 0, sizeof(vault_arg_list));

	output.mode = VAULT_ARG_MODE_INVALID;

	for (int i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "-u") || !strcmp(argv[i], "--upload")) {
			if (!_vault_args_check_format(argc, argv, i, 1)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.mode = VAULT_ARG_MODE_UPLOAD;
			output.filename = argv[i + 1];
		}

		if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--delete")) {
			if (!_vault_args_check_format(argc, argv, i, 1)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.mode = VAULT_ARG_MODE_DELETE;
			output.filename = argv[i + 1];
		}

		if (!strcmp(argv[i], "-l") || !strcmp(argv[i], "--list")) {
			if (!_vault_args_check_format(argc, argv, i, 0)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.mode = VAULT_ARG_MODE_LIST;
		}

		if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--password")) {
			if (!_vault_args_check_format(argc, argv, i, 1)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.password = argv[i + 1];
		}

		if (!strcmp(argv[i], "-s") || !strcmp(argv[i], "--share")) {
			if (!_vault_args_check_format(argc, argv, i, 3)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.filename = argv[i + 1];
			output.groupname = argv[i + 3];

			output.mode = VAULT_ARG_MODE_SHARE;
		}

		if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--config")) {
			if (!_vault_args_check_format(argc, argv, i, 0)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.mode = VAULT_ARG_MODE_CONFIG;
		}

		if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			if (!_vault_args_check_format(argc, argv, i, 0)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.mode = VAULT_ARG_MODE_HELP;
		}

		if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--revoke")) {
			if (!_vault_args_check_format(argc, argv, i, 2)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.mode = VAULT_ARG_MODE_REVOKE;

			output.filename = argv[i + 1];
			output.groupname = argv[i + 2];
		}

		if (!strcmp(argv[i], "-a") || !strcmp(argv[i], "--adduser")) {
			if (!_vault_args_check_format(argc, argv, i, 2)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.mode = VAULT_ARG_MODE_ADDUSER;

			output.username = argv[i + 1];
			output.groupname = argv[i + 2];
		}

		if (!strcmp(argv[i], "-du") || !strcmp(argv[i], "--deluser")) {
			if (!_vault_args_check_format(argc, argv, i, 2)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.mode = VAULT_ARG_MODE_DELUSER;

			output.username = argv[i + 1];
			output.groupname = argv[i + 2];
		}

		if (!strcmp(argv[i], "-ng") || !strcmp(argv[i], "--newgroup")) {
			if (!_vault_args_check_format(argc, argv, i, 1)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.mode = VAULT_ARG_MODE_NEWGROUP;

			output.groupname = argv[i + 1];
		}

		if (!strcmp(argv[i], "-dg") || !strcmp(argv[i], "--delgroup")) {
			if (!_vault_args_check_format(argc, argv, i, 1)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.mode = VAULT_ARG_MODE_DELGROUP;

			output.groupname = argv[i + 1];
		}
	}

	return output;
}

static int _vault_args_check_format(int argc, char** argv, int index, int arg_count)
{
	int valid_args = 0;

	for (int i = index + 1; i < argc; i++) {
		valid_args += (argv[i][0] != '-');

		if (valid_args == arg_count) {
			return 1;
		}
	}

	if (valid_args != arg_count) {
		printf("%s expects %d arguments.\n", argv[index], arg_count);
	}

	return (valid_args == arg_count);
}
