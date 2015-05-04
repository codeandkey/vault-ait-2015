#include "file.h"
#include "home.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

char* vault_file_read(char* filename, ...) {
	va_list args;
	va_start(args, filename);

	char fname[2048];
	vsnprintf(fname, 2048, filename, args);

	int file_size = 0;
	FILE* fd = fopen(fname, "r");

	if (!fd) {
		return NULL;
	}

	fseek(fd, 0, SEEK_END);
	file_size = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	char* output = malloc(file_size + 1);
	output[file_size] = 0;

	fread(output, 1, file_size, fd);
	fclose(fd);

	va_end(args);

	return output;
}

char* vault_file_read_size(int* file_size, char* filename, ...) {
	va_list args;
	va_start(args, filename);

	char fname[2048];
	vsnprintf(fname, 2048, filename, args);

	FILE* fd = fopen(fname, "r");

	if (!fd) {
		return NULL;
	}

	fseek(fd, 0, SEEK_END);
	*file_size = ftell(fd);
	fseek(fd, 0, SEEK_SET);

	char* output = malloc(*file_size + 1);
	output[*file_size] = 0;

	fread(output, 1, *file_size, fd);
	fclose(fd);

	va_end(args);

	return output;
}

int vault_file_write_home(char* filename, char* data, ...) {
	va_list args;
	va_start(args, data);

	char* home = vault_home_get();

	char filenamebuf[2048];
	snprintf(filenamebuf, 2048, "%s/%s", home, filename);

	FILE* fd = fopen(filenamebuf, "w");

	if (!fd) {
		printf("Failed to open %s for writing.\n", filenamebuf);
		return 0;
	}

	vfprintf(fd, data, args);
	fclose(fd);

	return 1;
}

int vault_file_write_home_raw(char* filename, char* data, int size) {
	char* home = vault_home_get();

	char filenamebuf[2048];
	snprintf(filenamebuf, 2048, "%s/%s", home, filename);

	FILE* fd = fopen(filenamebuf, "w");

	if (!fd) {
		printf("Failed to open %s for writing.\n", filenamebuf);
		return 0;
	}

	fwrite(data, 1, size, fd);
	fclose(fd);

	return 1;
}
