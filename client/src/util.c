#include "util.h"

#include <stdio.h>

void vault_util_printhex(char* buffer, int buffer_size)
{
	for (int i = 0; i < buffer_size; i++) {
		printf("%02X", (unsigned) buffer[i] & 0xFF);

		if (i != buffer_size - 1) {
			printf(" ");
		}
	}
}
