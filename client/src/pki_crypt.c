#include "pki_crypt.h"
#include "syscall.h"
#include "file.h"
#include "platform.h"

#include <stdlib.h>
#include <stdio.h>
	#include <string.h>

int vault_crypt_pki_genrsa(void)
{
#ifdef VAULT_PLATFORM_LINUX
	int result = 0;

	result = !vault_syscall("openssl genrsa -out /usr/local/share/vault/vault_pki_private.pem 4096");

	if (!result) {
		printf("Failed to generate RSA private key.\n");
		return 0;
	}

	result = !vault_syscall("openssl rsa -pubout -in /usr/local/share/vault/vault_pki_private.pem -out /usr/local/share/vault/vault_pki_public.pem");

	if (!result) {
		printf("Failed to extract RSA public key.\n");
		return 0;
	}

	return 1;
#else
	printf("RSA key generation not implemented for this platform!\n");
	return 0;
#endif
}

int vault_crypt_pki_encrypt(char* infile, char* outfile, char* keyfile)
{
	int result = 0;

	result = !vault_syscall("openssl rsautl -pubin -encrypt -in %s -out %s -inkey %s", infile, outfile, keyfile);

	return result;
}

int vault_crypt_pki_decrypt(char* infile, char* outfile)
{
	int result = 0;

	result = !vault_syscall("openssl rsautl -in %s -out %s -inkey /usr/local/share/vault/vault_pki_private.pem -decrypt", infile, outfile);

	return result;
}

int vault_crypt_pki_sign(char* infile, char* outsigfile)
{
	int result = 0;
	/* To sign a file, we encrypt the SHA256 hash. */

	result = !vault_syscall("openssl dgst -sha256 -out /usr/local/share/vault/vault_tmp_hash %s", infile);

	if (!result) {
		return 0;
	}

	result = !vault_syscall("openssl dgst -sign /usr/local/share/vault/vault_pki_private.pem -out %s /usr/local/share/vault/vault_tmp_hash", outsigfile);

	if (!result) {
		return 0;
	}

	result = !vault_syscall("rm -f /usr/local/share/vault/vault_tmp_hash");

	if (!result) {
		return 0;
	}

	return result;
}

int vault_crypt_pki_verify(char* infile, char* sigfile, char* keyfile)
{
	return 1;

	int result = 0;

	result = !vault_syscall("openssl dgst -sha256 -out /usr/local/share/vault/vault_tmp_hash %s", infile);

	if (!result) {
		return 0;
	}

	result = !vault_syscall("openssl dgst -verify %s -signature %s /usr/local/share/vault/vault_tmp_hash", keyfile, sigfile);

	vault_syscall("rm -f /usr/local/share/vault/vault_tmp_hash");

	if (!result) {
		return 0;
	}

	return 1;
}

vault_buffer vault_crypt_pki_encrypt_buf(char* inbuf, int inbufsize, char* keyfile)
{
	int result = 0;

	vault_buffer output;
	memset(&output, 0, sizeof(vault_buffer));

	/* First, we write the temporary file. */

	result = vault_file_write_raw("/usr/local/share/vault/vault_tmp_pki", inbuf, inbufsize);

	if (!result) {
		return output;
	}

	result = vault_crypt_pki_encrypt("/usr/local/share/vault/vault_tmp_pki", "/usr/local/share/vault/vault_tmp_pki2", keyfile);

	if (!result) {
		return output;
	}

	output = vault_file_read("/usr/local/share/vault/vault_tmp_pki2");

	vault_syscall("rm -f /usr/local/share/vault/vault_tmp_pki /usr/local/share/vault/vault_tmp_pki2");

	return output;
}

vault_buffer vault_crypt_pki_decrypt_buf(char* inbuf, int inbufsize)
{
	int result = 0;

	vault_buffer output;
	memset(&output, 0, sizeof(vault_buffer));

	/* First, we write the temporary file. */

	result = vault_file_write_raw("/usr/local/share/vault/vault_tmp_pki", inbuf, inbufsize);

	if (!result) {
		return output;
	}

	result = vault_crypt_pki_decrypt("/usr/local/share/vault/vault_tmp_pki", "/usr/local/share/vault/vault_tmp_pki2");

	if (!result) {
		return output;
	}

	output = vault_file_read("/usr/local/share/vault/vault_tmp_pki2");

	vault_syscall("rm -f /usr/local/share/vault/vault_tmp_pki /usr/local/share/vault/vault_tmp_pki2");

	return output;
}
