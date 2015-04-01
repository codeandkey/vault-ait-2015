#ifndef VAULT_CLI_H
#define VAULT_CLI_H

#include "vault_types.h"

#define VAULT_DBG 0
#define VAULT_CRT 1
#define VAULT_LOG 2

#define VAULT_LOG_FILE "vault.log"

void vault_print(uint8_t flags, const char* format, ...);

#endif
