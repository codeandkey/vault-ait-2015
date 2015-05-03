#pragma once

/* vault_crypt_aes() returns a NULL-terminated buffer of the new text. It is allocated on the heap, so free when you can. */

char* vault_crypt_aes(char* buffer, int buffer_size, char* key, int key_size, int encrypt); /* To encrypt, the last parameter should be 1. To decrypt, it should be 0. */
int vault_crypt_aes_file(char* infile, char* outfile, char* keyfile, int key_size, int encrypt);
