#include "vault_share.h"
#include "vault_cli.h"
#include "vault_encrypt.h"
#include "vault_hash.h"
#include "vault_rsa.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int vault_share(char* localfile, char* remote_user) {
	vault_print(VAULT_DBG, "Sharing localfile %s with remote user %s\n", localfile, remote_user);
	vault_print(VAULT_DBG, "Generating random key..\n");
	vault_print(VAULT_DBG, "Reading from /dev/random. You may need to move the mouse around, hit keys on the keyboard to generate entropy. \n");

	/* We will pull from /dev/random to generate the key. */

	char randomkey[33];

	FILE* dev_rand = fopen("/dev/random", "r");

	if (!dev_rand) {
		vault_print(VAULT_CRT, "Failed to open /dev/random for reading.\n");
		return 0;
	}

	fgets(randomkey, 32, dev_rand);
	fclose(dev_rand);

	for (int i = 0; i < 32; i++) {
		if (!randomkey[i]) {
			randomkey[i]++;
		}
	}

	randomkey[32] = 0;

	vault_print(VAULT_DBG, "Generated random key.\n");
	vault_print(VAULT_DBG, "Encrypting local file.\n");

	vault_encrypt_file_inplace(localfile, randomkey);

	vault_print(VAULT_DBG, "Encrypted local file.\n");
	vault_print(VAULT_DBG, "Retrieving remote user's key.\n");

	/* TODO : replace vaultio.conf with server register! */

	char user[33] = {0};
	vault_hash(remote_user, user, "@#$vault");
	user[32] = 0;

	char command[2048]= {0};

	snprintf(command, 2048, "vaultio download .vault_tmp_ruserkey %s", user);

	int result = system(command);

	if (result) {
		vault_print(VAULT_DBG, "Failed to download remote user key.");
		return 0;
	}

	vault_print(VAULT_DBG, "Downloaded remote user key.");
	vault_print(VAULT_DBG, "Encrypting random key..");

	FILE* keyfile = fopen(".vault_tmp_key", "w");

	if (!keyfile) {
		vault_print(VAULT_DBG, "Failed to write random key to file.\n");
		return 0;
	}

	fprintf(keyfile, randomkey);
	fclose(keyfile);

	vault_rsa_encrypt(".vault_tmp_key", ".vault_tmp_key_enc", ".vault_tmp_ruserkey");

	vault_print(VAULT_DBG, "Encrypted random key with RSA.");
	vault_print(VAULT_DBG, "Signing key for sending..");

	vault_rsa_sign(".vault_tmp_key_enc", ".vault_tmp_key_enc_sig");

	vault_print(VAULT_DBG, "Signed key. Uploading to server state register..");

	return 1;
}
