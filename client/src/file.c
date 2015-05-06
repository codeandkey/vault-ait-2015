#include "file.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

vault_buffer vault_file_read(char* filename, ...) {
	vault_buffer output_buffer;

	memset(&output_buffer, 0, sizeof(vault_buffer));

	va_list args;
	va_start(args, filename);

	char fname[2048];
	vsnprintf(fname, 2048, filename, args);

	int file_size = 0;
	FILE* fd = fopen(fname, "r");

	if (!fd) {
		return output_buffer;
	}

	fseek(fd, 0, SEEK_END);
	file_size = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	char* output = malloc(file_size + 1);
	output[file_size] = 0;

	fread(output, 1, file_size, fd);
	fclose(fd);

	va_end(args);

	output_buffer.ptr = output;
	output_buffer.size = file_size;

	return output_buffer;
}

int vault_file_write(char* filename, char* data, ...) {
	va_list args;
	va_start(args, data);

	FILE* fd = fopen(filename, "w");

	if (!fd) {
		printf("Failed to open %s for writing.\n", filename);
		return 0;
	}

	vfprintf(fd, data, args);
	fclose(fd);

	return 1;
}

int vault_file_write_raw(char* filename, char* data, int size) {
	FILE* fd = fopen(filename, "w");

	if (!fd) {
		printf("Failed to open %s for writing.\n", filename);
		return 0;
	}

	fwrite(data, 1, size, fd);
	fclose(fd);

	return 1;
}
