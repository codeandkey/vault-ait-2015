#pragma once

/* The group managerment subsystem allows easy modification of groups, members, uploading and revoking. */
/* The root group contains all files : revoking a file from root will delete it (and revoke it from all other groups) */

/* All of these functions return 1 on success and 0 on failure. */

int vault_group_create_root(void); /* This function creates the root group and should only be called during initial configuration. */
int vault_group_create(char* groupname); /* This function will create a group, generate a key for it and upload it. */
int vault_group_delete(char* groupname); /* This function will refuse to delete the root group. */

int vault_group_add_user(char* groupname, char* username); /* This function will add a user to an existing group. */
int vault_group_remove_user(char* groupname, char* username); /* This function will remove a user from an existing group. The user cannot be yourself. */

int vault_group_add_file(char* groupname, char* filename); /* This function will copy a file from the root group to <groupname>. */
int vault_group_get_file(char* username, char* groupname, char* filename, char* outfilename); /* This function will retrieve a file and store it in the target location. */

int vault_group_del_file(char*, char*);
