#ifndef VAULT_ENC_H
#define VAULT_ENC_H

/* This file simply provides initialization space for libgcrypt. */

int vault_enc_init(void);
void vault_enc_free(void);

#endif
