#ifndef VAULT_REMOTE_H
#define VAULT_REMOTE_H

/* The remote interface makes calls to vaultio for uploading and listing */

int vault_remote_upload(char* localfile, char* remotefile);
int vault_remote_download(char* localfile, char* remotefile);
int vault_remote_list(char** file_list, int* file_list_size);

#endif
