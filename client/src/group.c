#include "group.h"
#include "file.h"
#include "pki_crypt.h"
#include "util.h"
#include "syscall.h"
#include "platform.h"
#include "pki_crypt.h"
#include "sym_crypt.h"

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

		vault_file_write_raw("/usr/local/share/vault/vault_tmp_user_key_unenc", new_key, 32);
		vault_crypt_pki_encrypt("/usr/local/share/vault/vault_tmp_user_key_unenc", "/usr/local/share/vault/vault_tmp_user_key", "/usr/local/share/vault/vault_tmp_public_key");

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
	/* First, we download our key for the group. */

	char* username = vault_file_read("/usr/local/share/vault/vault_user").ptr;
	username[strlen(username) - 1] = 0;

	/* key_filename = "key/username" */

	char* key_filename = malloc(strlen(username) + 5);
	key_filename[strlen(username) + 4] = 0;
	memcpy(key_filename, "key/", 4);
	memcpy(key_filename + 4, username, strlen(username));

	printf("Crafted key filename : [%s]\n", key_filename);

	if (!_vault_download_safe(username, groupname, key_filename, "/usr/local/share/vault/af_temp_user", 1)) {
		printf("Failed to download encrypted group key.\n");
		return 0;
	}

	printf("Downloaded encrypted key.\n");

	if (!vault_crypt_pki_decrypt("/usr/local/share/vault/af_temp_user", "/usr/local/share/vault/af_temp_user_key")) {
		printf("Failed to decrypt key.\n");
		return 0;
	}

	printf("Decrypted group key.\n");
	printf("Encrypting file..\n");

	vault_buffer key = vault_file_read("/usr/local/share/vault/af_temp_user_key");

	if (!vault_encrypt_aes_file(filename, "/usr/local/share/vault/af_temp_file", key.ptr, key.size)) {
		printf("Failed to encrypt file with group key.\n");
		return 0;
	}

	printf("Encrypted file.\n");

	if (!_vault_upload_safe(groupname, "/usr/local/share/vault/af_temp_file", filename, 1)) {
		printf("Failed to upload file.\n");
		return 0;
	}

	vault_syscall("rm -f /usr/local/share/vault/af_temp_file /usr/local/share/vault/af_temp_user_key /usr/local/share/af_temp_user");

	free(username);
	free(key_filename);
	free(key.ptr);

	return 1;
}

int vault_group_get_file(char* username, char* groupname, char* filename, char* outfilename) {
	/* key_filename = "key/username" */

	char* owner = vault_file_read("/usr/local/share/vault/vault_user").ptr;
	owner[strlen(owner) - 1] = 0;

	char* key_filename = malloc(strlen(owner) + 5);
	key_filename[strlen(owner) + 4] = 0;
	memcpy(key_filename, "key/", 4);
	memcpy(key_filename + 4, owner, strlen(owner));

	printf("Crafted key filename : [%s]\n", key_filename);

	if (!_vault_download_safe(username, groupname, key_filename, "/usr/local/share/vault/af_temp_user", 1)) {
		printf("Failed to download encrypted group key.\n");
		return 0;
	}

	printf("Downloaded encrypted key.\n");

	if (!vault_crypt_pki_decrypt("/usr/local/share/vault/af_temp_user", "/usr/local/share/vault/af_temp_user_key")) {
		printf("Failed to decrypt key.\n");
		return 0;
	}

	printf("Decrypted group key.\n");
	printf("Downloading file..\n");

	if (!_vault_download_safe(username, groupname, filename, "/usr/local/share/vault/vault_tmp_file", 1)) {
		printf("Failed to download file.\n");
		return 0;
	}

	/* Decrypt the file. */

	vault_buffer key = vault_file_read("/usr/local/share/vault/af_temp_user_key");

	if (!vault_decrypt_aes_file("/usr/local/share/vault/vault_tmp_file", outfilename, key.ptr, key.size)) {
		printf("Failed to encrypt file with group key.\n");
		return 0;
	}

	vault_syscall("rm -f /usr/local/share/vault/af_temp_file /usr/local/share/vault/af_temp_user_key /usr/local/share/vault/af_temp_user /usr/local/share/vault/vault_tmp_file");

	free(key_filename);
	free(key.ptr);
	free(owner);

	return 1;
}

int vault_group_del_file(char* groupname, char* filename) { return 0; }

int vault_group_list(char* username, char* groupname) {
	if (vault_syscall("vaultio list %s", username)) {
		printf("Failed to list remote files.\n");
		return 0;
	}

	return 1;
}
