#include "file.h"

#include <stdlib.h>
#include <stdio.h>

char* vault_file_read(char* filename) {
	int file_size = 0;
	FILE* fd = fopen(filename, "r");

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

	return output;
}

char* vault_file_read_size(char* filename, int* file_size) {
	FILE* fd = fopen(filename, "r");

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

	return output;
}
