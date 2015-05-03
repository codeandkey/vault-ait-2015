#ifndef VAULT_RSA_H
#define VAULT_RSA_H

int vault_rsa_genkeys(void);

int vault_rsa_encrypt(char* infile, char* outfile, char* inkey);
int vault_rsa_decrypt(char* infile, char* outfile);
int vault_rsa_sign(char* infile, char* out_sigfile);
int vault_rsa_verify(char* infile, char* in_sigfile, char* inkey);

#endif
