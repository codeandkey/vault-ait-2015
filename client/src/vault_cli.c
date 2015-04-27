#include "vault_cli.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

static const char* VAULT_DEBUG_COLORS[2] = {
	"", /* normal printing */
	"\e[0;31m"  /* critical message printing */
};

void vault_print(uint8_t flags, const char* format, ...) {
	va_list args, fargs;
	va_start(args, format);

	va_copy(fargs, args);

	printf(VAULT_DEBUG_COLORS[flags & VAULT_CRT]);
	vprintf(format, args);
	printf("\e[0;39m");

	if (flags & VAULT_LOG) {
		FILE* log_file = fopen(VAULT_LOG_FILE, "a");

		vfprintf(log_file, format, fargs);
		fprintf(log_file, "\n");

		fclose(log_file);
	}

	va_end(args);

	if (flags & VAULT_CRT) {
		exit(1);
	}
}
