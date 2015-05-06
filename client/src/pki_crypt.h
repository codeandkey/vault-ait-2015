#pragma once

#include "types.h"

/* The PKI crypto functions operate on files. */

int vault_crypt_pki_genrsa(void);
int vault_crypt_pki_encrypt(char* infile, char* outfile, char* keyfile);
int vault_crypt_pki_decrypt(char* infile, char* outfile);
int vault_crypt_pki_verify(char* infile, char* infile_sig, char* keyfile);

vault_buffer vault_crypt_pki_encrypt_buf(char* inbuf, int inbufsize, char* keyfile);
vault_buffer vault_crypt_pki_decrypt_buf(char* inbuf, int inbufsize);
