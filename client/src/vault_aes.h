#ifndef VAULT_AES_H
#define VAULT_AES_H

/* Provides functions for AES symmetric encryption. */

int vault_encrypt_aes256(char* buffer, char* key); /* Both buffers should be NULL-terminated. */
int vault_decrypt_aes256(char* buffer, char* key);

#endif
