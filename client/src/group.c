#include "group.h"
#include "file.h"
#include "pki_crypt.h"
#include "util.h"
#include "syscall.h"
#include "platform.h"
#include "pki_crypt.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

static char* _vault_group_genkey(void);

int vault_group_create_root(void)
{
	return vault_group_create("root");
}

int vault_group_create(char* groupname)
{
	int sane = 1;

	sane &= strcmp(groupname, "public_key") ? 1 : 0;
	sane &= strcmp(groupname, "user_info") ? 1 : 0;
	sane &= strcmp(groupname, "user_groups") ? 1 : 0;

	sane &= !strstr(groupname, "/") ? 1 : 0;

	if (!sane) {
		printf("Group [%s] failed sanity check.\n", groupname);
		return 0;
	}

	char* username = vault_file_read("/usr/local/share/vault/vault_user").ptr;
	username[strlen(username) - 1] = 0; /* No newline. */

	if (vault_syscall("touch /usr/local/share/vault/tmp_empty")) {
		printf("Failed to touch empty file /usr/local/share/vault/tmp_empty.\n");
		return 0;
	}

	if (vault_syscall("vaultio upload /usr/local/share/vault/tmp_empty %s/group_list %s", groupname, username)) {
		printf("Failed to upload blank group list file.\n");
		return 0;
	}

	char nullbuf[37];
	memset(nullbuf, 0, 37);

	unsigned int timestamp = time(NULL);
	memcpy(nullbuf, &timestamp, sizeof(unsigned int));

	if (!vault_file_write_raw("/usr/local/share/vault/tmp_version", nullbuf, 37)) {
		printf("Failed to write temporary version file.\n");
		return 0;
	}

	if (vault_syscall("vaultio upload /usr/local/share/vault/tmp_version %s/group_version %s", groupname, username)) {
		printf("Failed to upload blank group version.\n");
		return 0;
	}

	vault_syscall("rm -f /usr/local/share/vault/tmp_version /usr/local/share/vault/tmp_empty");

	vault_group_add_user(groupname, NULL);

	printf("Created group %s/%s.\n", username, groupname);
	free(username);
	return 1;
}

int vault_group_delete(char* groupname)
{
	return 0;
}

int vault_group_add_user(char* groupname, char* username) {
	printf("Adding %s to %s\n", username, groupname);

	/* We have to do lots of things to safely finish this.
	 * Process :
	 * Check if modifier is owner
	 * Check if user is already in the group
	 * Add user to group member list, add group to user group list
	 * Generate a fresh group key
	 * Encrypt the new key for all members
	 * Update the group version file with a timestamp and incremented version ID
	 * Generate appropriate signatures for all files
	 */

	char* owner_username = vault_file_read("/usr/local/share/vault/vault_user").ptr;
	owner_username[strlen(owner_username) - 1] = 0;

	printf("owner_username = %s, username = %s\n", owner_username, username);

	int result = 0;

	result = !vault_syscall("vaultio download /usr/local/share/vault/vault_group_version %s/group_version %s", groupname, owner_username);

	if (!result) {
		printf("Failed to download group version.\n");
		return 0;
	}

	result = !vault_syscall("vaultio download /usr/local/share/vault/vault_group_list %s/group_list %s", groupname, owner_username);

	if (!result) {
		printf("Failed to download group member list.\n");
		return 0;
	}

	/* We want to ensure that the first username is our username. */

	char* group_list_buffer = vault_file_read("/usr/local/share/vault/vault_group_list").ptr;
	char* first_entry = strtok(group_list_buffer, "\n");

	if (username) {
		if (strcmp(first_entry, owner_username)) {
			printf("You are not the owner of the group! [you are %s, owner is %s]\n", first_entry, owner_username);
			return 0;
		}
	}

	/* From this point on, true-owners and foriegners are treated the same. */

	if (!username) {
		username = owner_username;
	}

	/* We need to check if the user is already in the group. */

	char* group_list_buf = vault_file_read("/usr/local/share/vault/vault_group_list").ptr;
	char* group_list_username = strtok(group_list_buf, "\n");

	while (group_list_username) {
		if (!strcmp(group_list_username, username)) {
			printf("User is already in group!\n");
			return 0;
		}

		group_list_username = strtok(NULL, "\n");
	}

	free(group_list_buf);

	/* The user is not in the group. */
	/* Add the user to the grouplist, and add the group to the user's list. */

	if (vault_syscall("echo %s >> /usr/local/share/vault/vault_group_list", username)) {
		printf("Failed to append new user to group list.\n");
		return 0;
	}

	if (vault_syscall("vaultio upload /usr/local/share/vault/vault_group_list %s/group_list %s", groupname, owner_username)) {
		printf("Failed to upload new group list.\n");
		return 0;
	}

	if (!vault_crypt_pki_sign("/usr/local/share/vault/vault_group_list", "/usr/local/share/vault/vault_group_list.sig")) {
		printf("Failed to sign group list.\n");
		return 0;
	}

	if (vault_syscall("vaultio upload /usr/local/share/vault/vault_group_list.sig %s/group_list.sig %s", groupname, owner_username)) {
		printf("Failed to upload group list signature.\n");
		return 0;
	}

	if (vault_syscall("vaultio download /usr/local/share/vault/vault_user_groups user_groups %s", username)) {
		printf("Failed to download user group file.\n");
		return 0;
	}

	if (vault_syscall("echo %s/%s >> /usr/local/share/vault/vault_user_groups", owner_username, groupname)) {
		printf("Failed to append group to user group list.\n");
		return 0;
	}

	if (vault_syscall("vaultio upload /usr/local/share/vault/vault_user_groups user_groups %s", username)) {
		printf("Failed to upload user group list.\n");
		return 0;
	}

	/* At this point, we've updated the user_groups file of the target user and also the group member list. */
	/* The next step is to generate a fresh key. */

	char* new_key = _vault_group_genkey();

	/* With the new key, encrypt/sign/upload it for each group member. */

	group_list_buf = vault_file_read("/usr/local/share/vault/vault_group_list").ptr;
	group_list_username = strtok(group_list_buf, "\n");

	while (group_list_username) {
		printf("Encrypting and uploading key for user %s\n", group_list_username);

		if (vault_syscall("vaultio download /usr/local/share/vault/vault_tmp_public_key public_key %s", group_list_username)) {
			printf("Failed to download user public key.\n");
			continue;
		}

		vault_buffer user_key = vault_crypt_pki_encrypt_buf(new_key, 32, "/usr/local/share/vault/vault_tmp_public_key");

		vault_file_write_raw("/usr/local/share/vault/vault_tmp_user_key", user_key.ptr, user_key.size);

		if (!vault_crypt_pki_sign("/usr/local/share/vault/vault_tmp_user_key", "/usr/local/share/vault/vault_tmp_user_key.sig")) {
			printf("Failed to sign user key.\n");
			continue;
		}

		if (vault_syscall("vaultio upload /usr/local/share/vault/vault_tmp_user_key %s/key/%s %s", groupname, username, owner_username)) {
			printf("Failed to upload user key.\n");
			continue;
		}

		if (vault_syscall("vaultio upload /usr/local/share/vault/vault_tmp_user_key.sig %s/key/%s.sig %s", groupname, username, owner_username)) {
			printf("Failed to upload user key signature.\n");
			continue;
		}

		vault_syscall("rm -f /usr/local/share/vault/vault_tmp_public_key /usr/local/share/vault/vault_tmp_user_key /usr/local/share/vault/vault_tmp_user_key.sig");

		group_list_username = strtok(NULL, "\n");
	}

	printf("Updating group version.\n");

	char* tmp_group_version = vault_file_read("/usr/local/share/vault/vault_group_version").ptr;

	char group_version[36] = {0};
	unsigned int timestamp = time(NULL);

	uint32_t* group_version_id = (uint32_t*) (group_version + 32);
	uint32_t* store_version_id = (uint32_t*) (tmp_group_version + 32);

	*group_version_id = *store_version_id + 1;
	memcpy(group_version, &timestamp, sizeof(unsigned int));

	free(tmp_group_version);

	printf("Uploading new group version..\n");

	vault_file_write_raw("/usr/local/share/vault/vault_group_version", group_version, 36);

	if (!vault_crypt_pki_sign("/usr/local/share/vault/vault_group_version", "/usr/local/share/vault/vault_group_version.sig")) {
		printf("Failed to sign group version.\n");
		return 0;
	}

	if (vault_syscall("vaultio upload /usr/local/share/vault/vault_group_version %s/group_version %s", groupname, username)) {
		printf("Failed to upload updated group version.\n");
		return 0;
	}

	if (vault_syscall("vaultio upload /usr/local/share/vault/vault_group_version.sig %s/group_version.sig %s", groupname, username)) {
		printf("Failed to upload updated group version signature.\n");
		return 0;
	}

	printf("Cleaning up temp files.\n");

	vault_syscall("rm -f /usr/local/share/vault/vault_group_version /usr/local/share/vault/vault_user_groups /usr/local/share/vault/vault_group_list");

	free(new_key);
	free(owner_username);
	free(group_list_buffer);
	return 1;
}

char* _vault_group_genkey(void)
{
	char* groupkey = malloc(33);
	groupkey[32] = 0;

#ifdef VAULT_BUILD_DEBUG
	FILE* dev_rand = fopen("/dev/urandom", "r");
#else
	FILE* dev_rand = fopen("/dev/random", "r");
#endif

	if (!dev_rand) {
		printf("Failed to open /dev/random for reading.\n");
		return NULL;
	}

	fread(groupkey, 1, 32, dev_rand);
	fclose(dev_rand);

	return groupkey;
}

int vault_group_create_quick(char* groupname, char* username) {
	if (!vault_group_create(groupname)) {
		return 0;
	}

	if (vault_group_add_user(groupname, username)) {
		return 0;
	}

	return 1;
}
