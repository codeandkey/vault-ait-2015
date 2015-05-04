#include "home.h"
#include "platform.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef VAULT_PLATFORM_LINUX
#include <unistd.h>
#include <sys/types.h>
#elif defined(VAULT_PLATFORM_WIN32) || defined(VAULT_PLATFORM_OSX)
#endif

char* vault_home_get(void) {
#ifdef VAULT_PLATFORM_LINUX
	return getenv("HOME");
#elif defined(VAULT_PLATFORM_WIN32) || defined(VAULT_PLATFORM_OSX)
	printf("Home directory retrieval not yet implemented for this platform!\n");
	return "";
#endif
}
