#pragma once

#define VAULT_ARG_MODE_INVALID -1
#define VAULT_ARG_MODE_UPLOAD 0 /* User is uploading a file to the root group. */
#define VAULT_ARG_MODE_DELETE 1 /* User is revoking a file from the root group. */
#define VAULT_ARG_MODE_GET 2 /* User is retrieving a file from a certain group (arg1 = "ownername/groupname", arg2 = filename). */
#define VAULT_ARG_MODE_SHARE 3 /* User is adding a file from the root group to another group. */
#define VAULT_ARG_MODE_REVOKE 4 /* User is revoking a file from a group. */
#define VAULT_ARG_MODE_ADDUSER 5 /* User is adding a user to a group. */
#define VAULT_ARG_MODE_DELUSER 6 /* User is removing another user from a group. */
#define VAULT_ARG_MODE_HELP 7 /* User is getting help. */
#define VAULT_ARG_MODE_CONFIG 8 /* User is running first-time config. */
#define VAULT_ARG_MODE_LIST 9 /* User is listing files and groups. */
#define VAULT_ARG_MODE_NEWGROUP 10
#define VAULT_ARG_MODE_DELGROUP 11

/* Valid mode formats:
 * (-u) --upload <filename>
 * (-d) --delete <filename>
 * (-g) --get <username> <groupname> <filename>
 * (-s) --share <filename> <groupname>
 * (-l) --list
 * (-c) --config
 * (-r) --revoke <filename> <groupname>
 * (-au) --adduser <username> <groupname>
 * (-du) --deluser <username> <groupname>
 * (-p) --password <password>
 * (-ng) <groupname>
 * (-dg) <groupname>
 */

struct _vault_arg_list {
	int mode;
	char* filename, *groupname, *username, *password;
};

typedef struct _vault_arg_list vault_arg_list;

vault_arg_list vault_args_get(int argc, char** argv);
