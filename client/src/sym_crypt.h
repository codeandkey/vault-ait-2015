#pragma once

/* vault_crypt_aes() returns a NULL-terminated buffer of the new text. It is allocated on the heap, so free when you can. */

char* vault_encrypt_aes(char* buffer, int buffer_size, char* key_data, int key_size);
char* vault_decrypt_aes(char* buffer, int buffer_size, char* key_data, int key_size);

int vault_encrypt_aes_file(char* infile, char* outfile, char* key_data, int key_size);
int vault_decrypt_aes_file(char* infile, char* outfile, char* key_data, int key_size);
