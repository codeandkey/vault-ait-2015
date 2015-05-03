#include "syscall.h"
#include "platform.h"

#include <stdlib.h>
#include <stdio.h>

static int _vault_syscall_system(char* buffer)
{
	/* Commands are assumed to be sanitized.. */
	/* These are all the same. For now.. */

#if defined(VAULT_PLATFORM_WIN32)
	return system(buffer);
#elif defined(VAULT_PLATFORM_LINUX)
	return system(buffer);
#elif defined(VAULT_PLATFORM_OSX)
	return system(buffer);
#endif
}

int vault_syscall(const char* program_fmt, ...)
{
	va_list args;
	va_start(args, program_fmt);

	char execute_buffer[2048] = {0};
	vsnprintf(execute_buffer, 2048, program_fmt, args);

	int output = _vault_syscall_system(execute_buffer);

	va_end(args);

	return output;
}
