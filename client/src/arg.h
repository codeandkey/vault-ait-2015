#pragma once

#define VAULT_ARG_MODE_INVALID -1
#define VAULT_ARG_MODE_GET 0
#define VAULT_ARG_MODE_PUT 1
#define VAULT_ARG_MODE_LIST 2
#define VAULT_ARG_MODE_SHARE 3
#define VAULT_ARG_MODE_CONFIG 4
#define VAULT_ARG_MODE_HELP 5
#define VAULT_ARG_MODE_CONFIGSHARE 6
#define VAULT_ARG_MODE_REVOKE 7
#define VAULT_ARG_MODE_ENCRYPT 8
#define VAULT_ARG_MODE_DECRYPT 9

struct _VaultArgList {
	int mode;
	char* infile, *outfile, *password;
};

typedef struct _VaultArgList VaultArgList;

VaultArgList vault_args_get(int argc, char** argv);
