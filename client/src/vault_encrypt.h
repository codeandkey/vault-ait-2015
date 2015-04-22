#ifndef VAULT_ENCRYPT_H
#define VAULT_ENCRYPT_H

int vault_encrypt_file(char* infile, char* outfile, char* key_hash);
int vault_encrypt_file_inplace(char* infile, char* key_hash);

int vault_decrypt_file(char* infile, char* outfile, char* key_hash);
int vault_decrypt_file_inplace(char* infile, char* key_hash);

#endif
