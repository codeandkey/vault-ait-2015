#include "user.h"
#include "syscall.h"

int vault_user_getkey(char* username, char* filename)
{
	return vault_syscall("vaultio download public_key %s %s", filename, username);
}
