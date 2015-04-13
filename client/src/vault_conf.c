#include "vault_conf.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <openssl/sha.h>

int vault_conf_prompt(void) {
	const char* salt = "@#$vault";

	char azure_account_name[1024] = {0}, azure_account_key[1024] = {0};
	unsigned char username[1024] = {0}, globalkey[1024];

	printf("[vault_conf_prompt] Starting interactive configuration generation..\n\n");

	system("clear");

	printf("> Welcome to the Vault configuration generator.\n");
	printf("> [Defaulting to Azure configuration.]\n");

	printf("> Enter the Azure storage account name : ");
	fgets(azure_account_name, 1024, stdin);

	azure_account_name[strlen(azure_account_name) - 1] = 0; /* Kill the newline */

	printf("> Enter the Azure storage account access key : ");
	fgets(azure_account_key, 1024, stdin);

	azure_account_key[strlen(azure_account_key) - 1] = 0;

	printf("> [Azure connection configuration complete. Starting user configuration.]\n");
	printf("> Please enter a username : ");

	fgets((char*) username, 1024, stdin);
	username[strlen((char*) username) - 1] = 0;

	printf("> Please enter a passphrase (globalkey) : ");

	fgets((char*) globalkey, 1024, stdin);
	globalkey[strlen((char*) globalkey) - 1] = 0;

	system("clear");

	printf("> All credentials received.\n");
	printf("> Generating user container..\n");

	unsigned char container[SHA_DIGEST_LENGTH] = {0};

	unsigned char* username_input = malloc(sizeof(unsigned char) * strlen(salt) * 2 + strlen((char*) username));

	strcpy((char*) username_input, salt);
	strcpy((char*) username_input + strlen(salt), (char*) username);
	strcpy((char*) username_input + strlen(salt) + strlen((char*) username), salt);

	printf("> [username_input = %s]\n", username_input);

	SHA1(username_input, strlen((char*) username_input), container);

	printf("> .. Done.\n");
	printf("> Generating global key..\n");

	unsigned char globalkey_final[SHA_DIGEST_LENGTH] = {0};

	SHA1(globalkey, strlen((char*) globalkey), globalkey_final);

	printf("> .. Done.\n");
	printf("> Writing vaultio.conf..\n");

	FILE* io_conf = fopen("vaultio.conf", "w");

	if (!io_conf) {
		printf("> Failed to open vaultio.conf for writing.\n");
		return 0;
	}

	fprintf(io_conf, "azure_account_name=%s\n", azure_account_name);
	fprintf(io_conf, "azure_account_key=%s\n", azure_account_key);
	fprintf(io_conf, "azure_container=%s\n", container);
	fprintf(io_conf, "protocol=azure");

	fclose(io_conf);

	printf("> .. Done.\n");
	printf("> Changing vaultio.conf read permissions..\n");

	system("chmod go-rwx vaultio.conf");

	printf("> .. Done\n");

	printf("> Writing vault.conf..\n");

	FILE* vault_conf = fopen("vault.conf", "w");

	if (!vault_conf) {
		printf("> Failed to open vault.conf for writing.\n");
		return 0;
	}

	fprintf(vault_conf, "globalkey=%s\n", globalkey_final);
	fclose(vault_conf);

	printf("> .. Done.\n");
	printf("> Changing vault.conf read permissions..\n");

	system("chmod go-rwx vault.conf");

	printf("> .. Done.\n");
	printf("> Configuration generation completed successfully.\n");

	return 1;
}
