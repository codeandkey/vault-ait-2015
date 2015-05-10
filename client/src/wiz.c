#include "wiz.h"
#include "syscall.h"
#include "platform.h"
#include "hash.h"
#include "file.h"
#include "util.h"
#include "group.h"
#include "pki_crypt.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int vault_wiz(void)
{
	printf("Starting Vault configuration wizard for platform %s.. \n", VAULT_PLATFORM_STRING);

#if defined(VAULT_PLATFORM_WIN32) || defined(VAULT_PLATFORM_OSX)
	printf("Configuration wizard has not been implemented for this platform yet!\n");
	return 0;
#endif
	int result = 1;

	printf("Initializing local directories in /usr/local/share/vault.\n");

	result &= !vault_syscall("mkdir -p /usr/local/share/vault");

	if (!result) {
		printf("Failed to create local directory.\n");
		return 0;
	}

	printf("Generating local PKI encryption keys.\n");

	result &= vault_crypt_pki_genrsa();

	if (!result) {
		printf("Failed to generate PKI keys.\n");
		return 0;
	}

	printf("Please enter a username for remote storage : ");
	char username[2048];
	memset(username, 0, 2048);

	fgets(username, 2048, stdin);

	if (!vault_file_write("/usr/local/share/vault/vault_user", username)) {
		return 0;
	}

	result &= !vault_syscall("vaultio upload /usr/local/share/vault/vault_pki_public.pem info/public_key %s", username);

	if (!result) {
		printf("Failed to upload public key.\n");
		return 0;
	}

	printf("Touching remote group index.\n");

	result &= !vault_syscall("touch /usr/local/share/vault/tmp");
	result &= !vault_syscall("vaultio upload /usr/local/share/vault/tmp info/user_groups %s", username);

	if (!vault_crypt_pki_sign("/usr/local/share/vault/tmp", "/usr/local/share/vault/tmp.sig")) {
		printf("Failed to sign usergroups file.\n");
		return 0;
	}

	result &= !vault_syscall("vaultio upload /usr/local/share/vault/tmp.sig info/user_groups.sig %s", username);

	if (!result) {
		printf("Failed to upload blank usergroups file.\n");
		return 0;
	}

	vault_syscall("rm /usr/local/share/vault/tmp");

	printf("Generating root group.\n");

	result &= vault_group_create_root();

	if (!result) {
		printf("Failed to create root group.\n");
		return 0;
	}

	return 1;
}
