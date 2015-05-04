#include "wiz.h"
#include "syscall.h"
#include "platform.h"
#include "hash.h"
#include "file.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int vault_wiz(void)
{
	printf("Starting Vault configuration wizard for platform %s.. \n", VAULT_PLATFORM_STRING);

#if defined(VAULT_PLATFORM_WIN32) || defined(VAULT_PLATFORM_OSX)
	printf("Configuration wizard has not been implemented for this platform yet!\n");
#endif

	FILE* dev_rand = NULL;
	char random_bytes[33] = {0};

	int result = 1;

	printf("Generating local PKI encryption keys.\n");

	result &= !vault_syscall("openssl genrsa -out vault_pki_private.pem 4096");
	result &= !vault_syscall("openssl rsa -pubout -in vault_pki_private.pem -out vault_pki_public.pem");

	if (!result) {
		printf("Failed to generate PKI keys.\n");
		return 0;
	}

	printf("Generating local symmetric keys.\n");

#ifdef VAULT_BUILD_DEBUG
	dev_rand = fopen("/dev/urandom", "r"); /* /dev/random is really slow (blocking device) and we don't need that for debugging. */
#else
	dev_rand = fopen("/dev/random", "r");
#endif

	if (!dev_rand) {
		printf("Failed to open /dev/random.\n");
		return 0;
	}

	printf("Reading securely from /dev/random. You may need to generate some entropy. (move the mouse, use the keyboard, use the disks)\n");

	fread(random_bytes, 1, 32, dev_rand);
	fclose(dev_rand);

	random_bytes[32] = 0;

	if (!vault_file_write_home_raw(".vault_key", random_bytes, 32)) {
		return 0;
	}

	printf("Please enter a username for remote storage : ");
	char username[2048];
	memset(username, 0, 2048);

	fgets(username, 2048, stdin);

	if (!vault_file_write_home(".vault_user", username)) {
		return 0;
	}

	printf("Uploading public key to remote storage..\n");

	result &= !vault_syscall("vaultio upload ~/.vault_key public_key %s", username);

	if (!result) {
		printf("Failed to upload public key.\n");
		return 0;
	}

	return 1;
}
