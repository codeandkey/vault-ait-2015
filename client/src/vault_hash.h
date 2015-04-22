#ifndef VAULT_HASH_H
#define VAULT_HASH_H

/* The hash type is a SHA-256 hash. The output buffer should be 32 bytes long. */

void vault_hash(char* input, char* output, char* salt);

#endif
