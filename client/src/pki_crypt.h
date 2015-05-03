#pragma once

/* The PKI crypto functions operate on files. */

int vault_crypt_pki_genrsa(void);
int vault_crypt_pki_encrypt(char* infile, char* outfile, char* keyfile);
int vault_crypt_pki_decrypt(char* infile, char* outfile);
int vault_crypt_pki_verify(char* infile, char* infile_sig, char* keyfile);
