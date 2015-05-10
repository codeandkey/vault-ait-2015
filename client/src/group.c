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

static int _vault_touch_remote(char* groupname, char* filename, int sig);

static int _vault_genkeys(char* groupname); /* Generates a key, uploads and signs it for all users. */

static int _vault_download_safe(char* username, char* groupname, char* filename, char* target_filename, int sig); /* Downloads the file and checks the signature. */
static int _vault_upload_safe(char* groupname, char* filename, char* target_filename, int sig); /* Uploads the file along with the signature. */

static int _vault_add_line(char* username, char* groupname, char* filename, char* line, int sig);
static int _vault_del_line(char* username, char* groupname, char* filename, char* line, int sig);

int vault_group_create_root(void)
{
	return vault_group_create("root");
}

int vault_group_create(char* groupname)
{
	int sane = 1;

	sane &= strcmp(groupname, "info") ? 1 : 0;

	sane &= !strstr(groupname, "/") ? 1 : 0;

	if (!sane) {
		printf("Group [%s] failed sanity check.\n", groupname);
		return 0;
	}

	char* username = vault_file_read("/usr/local/share/vault/vault_user").ptr;
	username[strlen(username) - 1] = 0; /* No newline. */

	_vault_touch_remote(groupname, "group_list", 1);
	_vault_touch_remote(groupname, "group_version", 1);

	printf("Touched remote group files.\n");
	printf("Adding user..\n");

	vault_group_add_user(groupname, username);

	printf("Created group %s/%s.\n", username, groupname);
	free(username);
	return 1;
}

int vault_group_delete(char* groupname)
{
	return 0;
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

int _vault_genkeys(char* groupname) {
	char* username = vault_file_read("/usr/local/share/vault/vault_user").ptr;
	username[strlen(username) - 1] = 0;

	if (vault_syscall("vaultio download /usr/local/share/vault/vault_group_list %s/group_list %s", groupname, username)) {
		printf("Failed to download group list.\n");
		return 0;
	}

	char* new_key = _vault_group_genkey();

	/* We iterate through each user.. */

	char* group_list_buf = vault_file_read("/usr/local/share/vault/vault_group_list").ptr;
	char* group_list_username = strtok(group_list_buf, "\n");

	printf("Starting key upload loop.\n");

	while (group_list_username) {
		printf("Encrypting and uploading key for user %s\n", group_list_username);

		if (vault_syscall("vaultio download /usr/local/share/vault/vault_tmp_public_key info/public_key %s", group_list_username)) {
			printf("Failed to download user public key.\n");
			continue;
		}

		vault_buffer user_key = vault_crypt_pki_encrypt_buf(new_key, 32, "/usr/local/share/vault/vault_tmp_public_key");

		vault_file_write_raw("/usr/local/share/vault/vault_tmp_user_key", user_key.ptr, user_key.size);

		if (!vault_crypt_pki_sign("/usr/local/share/vault/vault_tmp_user_key", "/usr/local/share/vault/vault_tmp_user_key.sig")) {
			printf("Failed to sign user key.\n");
			continue;
		}

		if (vault_syscall("vaultio upload /usr/local/share/vault/vault_tmp_user_key %s/key/%s %s", groupname, group_list_username, username)) {
			printf("Failed to upload user key.\n");
			continue;
		}

		if (vault_syscall("vaultio upload /usr/local/share/vault/vault_tmp_user_key.sig %s/key/%s.sig %s", groupname, group_list_username, username)) {
			printf("Failed to upload user key signature.\n");
			continue;
		}

		vault_syscall("rm -f /usr/local/share/vault/vault_tmp_public_key /usr/local/share/vault/vault_tmp_user_key /usr/local/share/vault/vault_tmp_user_key.sig");

		group_list_username = strtok(NULL, "\n");
	}

	printf("Finished uploading keys.\n");

	vault_syscall("rm -f /usr/local/share/vault/vault_group_list");

	free(new_key);
	free(username);
	free(group_list_buf);
	free(group_list_username);

	return 1;
}

int _vault_touch_remote(char* groupname, char* filename, int sig) {
	char* username = vault_file_read("/usr/local/share/vault/vault_user").ptr;
	username[strlen(username) - 1] = 0;

	vault_syscall("touch /usr/local/share/vault/vault_tmp_empty");

	if (vault_syscall("vaultio upload /usr/local/share/vault/vault_tmp_empty %s/%s %s", groupname, filename, username)) {
		printf("Failed to touch remote file.\n");
		return 0;
	}

	if (sig) {
		vault_crypt_pki_sign("/usr/local/share/vault/vault_tmp_empty", "/usr/local/share/vault/vault_tmp_empty.sig");

		if (vault_syscall("vaultio upload /usr/local/share/vault/vault_tmp_empty.sig %s/%s.sig %s", groupname, filename, username)) {
			printf("Failed to upload remote signature.\n");
			return 0;
		}
	}

	vault_syscall("rm -f /usr/local/share/vault/vault_tmp_empty /usr/local/share/vault/vault_tmp_empty.sig");
	free(username);

	return 0;
}

int _vault_download_safe(char* username, char* groupname, char* filename, char* target_filename, int sig) {
	printf("[debug] Downloading %s/%s from user %s to local file %s\n", groupname, filename, username, target_filename);

	if (vault_syscall("vaultio download /usr/local/share/vault/vault_public_key info/public_key %s", username)) {
		printf("Failed to download public key.\n");
		return 0;
	}

	if (vault_syscall("vaultio download %s %s/%s %s", target_filename, groupname, filename, username)) {
		printf("Failed to download target file.\n");
		return 0;
	}

	if (sig) {
		if (vault_syscall("vaultio download /usr/local/share/vault/vault_tmp_file.sig %s/%s.sig %s", groupname, filename, username)) {
			printf("Failed to download target file signature.\n");
			return 0;
		}

		if (!vault_crypt_pki_verify(target_filename, "/usr/local/share/vault/vault_tmp_file.sig", "/usr/local/share/vault/vault_public_key")) {
			printf("File verification failed.\n");
			return 0;
		}
	}

	vault_syscall("rm -f /usr/local/share/vault/vault_tmp_file.sig /usr/local/share/vault/vault_public_key");

	return 1;
}

int _vault_upload_safe(char* groupname, char* filename, char* target_filename, int sig) {
	printf("[debug] Uploading %s to %s/%s\n", filename, groupname, target_filename);

	char* username = vault_file_read("/usr/local/share/vault/vault_user").ptr;
	username[strlen(username) - 1] = 0;

	if (!vault_crypt_pki_sign(filename, "/usr/local/share/vault/vault_tmp_sig")) {
		printf("Failed to sign file.\n");
		return 0;
	}

	if (vault_syscall("vaultio upload %s %s/%s %s", filename, groupname, target_filename, username)) {
		printf("Failed to upload file.\n");
		return 0;
	}

	if (sig) {
		if (vault_syscall("vaultio upload /usr/local/share/vault/vault_tmp_sig %s/%s.sig %s", groupname, target_filename, username)) {
			printf("Failed to upload file signature.\n");
			return 0;
		}
	}

	vault_syscall("rm -f /usr/local/share/vault/vault_tmp_sig");

	free(username);
	return 1;
}

int vault_group_add_user(char* groupname, char* username) {
	/* First, add the user to the group list. */

	char* owner = vault_file_read("/usr/local/share/vault/vault_user").ptr;
	owner[strlen(owner) - 1] = 0;

	char* group_list_line = malloc(strlen(owner) + strlen(groupname) + 2);

	memcpy(group_list_line, owner, strlen(owner));
	group_list_line[strlen(owner)] = '/';
	memcpy(group_list_line + strlen(owner) + 1, groupname, strlen(groupname));
	group_list_line[strlen(owner) + strlen(groupname) + 1] = 0;

	/* We have smaller subroutines, so this can look a bit nicer. */
	/* Add a line to the user's group list. */

	if (!_vault_add_line(username, "info", "user_groups", group_list_line, 0)) {
		printf("Failed to add line to user groups file.\n");
		return 0;
	}

	/* Add the user to the group members file. */

	if (!_vault_add_line(owner, groupname, "group_list", username, 1)) {
		printf("Failed to add user to group member list.\n");
		return 0;
	}

	if (!_vault_genkeys(groupname)) {
		printf("Failed to regenerate keys.\n");
		return 0;
	}

	free(owner);
	free(group_list_line);
	return 1;
}

int vault_group_remove_user(char* groupname, char* username) {
	/* First, add the user to the group list. */

	char* owner = vault_file_read("/usr/local/share/vault/vault_user").ptr;
	owner[strlen(owner) - 1] = 0;

	char* group_list_line = malloc(strlen(owner) + strlen(groupname) + 2);

	memcpy(group_list_line, owner, strlen(owner));
	group_list_line[strlen(owner)] = '/';
	memcpy(group_list_line + strlen(owner) + 1, groupname, strlen(groupname));
	group_list_line[strlen(owner) + strlen(groupname) + 1] = 0;

	/* We have smaller subroutines, so this can look a bit nicer. */
	/* Add a line to the user's group list. */

	if (!_vault_del_line(username, "info", "user_groups", group_list_line, 0)) {
		printf("Failed to add line to user groups file.\n");
		return 0;
	}

	/* Add the user to the group members file. */

	if (!_vault_del_line(owner, groupname, "group_list", username, 1)) {
		printf("Failed to add user to group member list.\n");
		return 0;
	}

	if (!_vault_genkeys(groupname)) {
		printf("Failed to regenerate keys.\n");
		return 0;
	}

	free(owner);
	free(group_list_line);
	return 1;
}

int _vault_add_line(char* username, char* groupname, char* filename, char* line, int sig) {
	printf("[debug] Adding %s to %s:%s/%s, sig %d\n", line, username, groupname, filename, sig);

	if (!_vault_download_safe(username, groupname, filename, "/usr/local/share/vault/al_file", sig)) {
		printf("Failed to get file.\n");
		return 0;
	}

	if (vault_syscall("echo %s >> /usr/local/share/vault/al_file", line)) {
		printf("Failed to add line.\n");
		return 0;
	}

	if (!_vault_upload_safe(groupname, "/usr/local/share/vault/al_file", filename, sig)) {
		printf("Failed to upload file.\n");
		return 0;
	}

	vault_syscall("rm -f /usr/local/share/vault/al_file");
	return 1;
}

int _vault_del_line(char* username, char* groupname, char* filename, char* line, int sig) {
	if (!_vault_download_safe(username, groupname, filename, "/usr/local/share/vault/dl_file", sig)) {
		printf("Failed to get file.\n");
		return 0;
	}

	if (vault_syscall("sed '/%s/d' /usr/local/share/vault/dl_file > /usr/local/share/vault/dl_file", line)) {
		printf("Failed to delete line.\n");
		return 0;
	}

	if (!_vault_upload_safe(groupname, "/usr/local/share/vault/dl_file", filename, sig)) {
		printf("Failed to upload file.\n");
		return 0;
	}


	vault_syscall("rm -f /usr/local/share/vault/dl_file");
	return 1;
}

int vault_group_add_file(char* groupname, char* filename) {
	/* We want to encrypt a file with the key and then upload it. */

	
}
