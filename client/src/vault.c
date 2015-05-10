#include "vault.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int _test_element(int result, const char* id);

int main(int argc, char** argv)
{
	int output = 0;

	vault_arg_list args = vault_args_get(argc, argv);

	printf("Executing on platform %s\n", VAULT_PLATFORM_STRING);
	printf("Starting mode %d, filename = [%s], groupname = [%s], username = [%s], password = [%s]\n", args.mode, args.filename, args.groupname, args.username, args.password);

	switch (args.mode) {
	case VAULT_ARG_MODE_INVALID:
		printf("Invalid arguments!\n");
	case VAULT_ARG_MODE_HELP:
		printf("Vault client, version %s\n", VAULT_VERSION_STRING);
		printf("Arguments :\n");
		break;
	case VAULT_ARG_MODE_CONFIG:
		output = _test_element(vault_wiz(), "configuration");
		break;
	case VAULT_ARG_MODE_NEWGROUP:
		output = _test_element(vault_group_create(args.groupname), "group creation");
		break;
	case VAULT_ARG_MODE_DELGROUP:
		output = _test_element(vault_group_delete(args.groupname), "group deletion");
		break;
	case VAULT_ARG_MODE_ADDUSER:
		output = _test_element(vault_group_add_user(args.groupname, args.username), "user addition");
		break;
	case VAULT_ARG_MODE_DELUSER:
		output = _test_element(vault_group_remove_user(args.groupname, args.username), "user deletion");
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
