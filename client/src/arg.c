#include "arg.h"

#include <stdlib.h>
#include <string.h>

static int _vault_args_check_format(int argc, char** argv, int i, int arg_count);

VaultArgList vault_args_get(int argc, char** argv)
{
	VaultArgList output;

	output.mode = VAULT_ARG_MODE_INVALID;
	output.infile = output.outfile = output.password = NULL;

	for (int i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "-g") || !strcmp(argv[i], "--get")) {
			if (!_vault_args_check_format(argc, argv, i, 2)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.mode = VAULT_ARG_MODE_GET;
			output.infile = argv[i + 1];
			output.outfile = argv[i + 2];
		}

		if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--put")) {
			if (!_vault_args_check_format(argc, argv, i, 2)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.mode = VAULT_ARG_MODE_PUT;
			output.infile = argv[i + 1];
			output.outfile = argv[i + 2];
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
			if (!_vault_args_check_format(argc, argv, i, 2)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.infile = argv[i + 1];
			output.outfile = argv[i + 2];
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

		if (!strcmp(argv[i], "-u") || !strcmp(argv[i], "--configshare")) {
			if (!_vault_args_check_format(argc, argv, i, 0)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.mode = VAULT_ARG_MODE_HELP;
		}

		if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--revoke")) {
			if (!_vault_args_check_format(argc, argv, i, 0)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.mode = VAULT_ARG_MODE_REVOKE;
		}

		if (!strcmp(argv[i], "-e") || !strcmp(argv[i], "--encrypt")) {
			if (!_vault_args_check_format(argc, argv, i, 2)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.mode = VAULT_ARG_MODE_ENCRYPT;
			output.infile = argv[i + 1];
			output.outfile = argv[i + 2];
		}

		if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--decrypt")) {
			if (!_vault_args_check_format(argc, argv, i, 2)) {
				output.mode = VAULT_ARG_MODE_INVALID;
				return output;
			}

			output.mode = VAULT_ARG_MODE_DECRYPT;
			output.infile = argv[i + 1];
			output.outfile = argv[i + 2];
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

	return (valid_args == arg_count);
}
