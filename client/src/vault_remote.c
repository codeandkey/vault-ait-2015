#include "vault_remote.h"

static int execute_program(char* program_name);

int vault_remote_upload(char* localfile, char* remotefile) {
}

int vault_remote_download(char* localfile, char* remotefile) {
}

int vault_remote_list(char** file_list, int* file_list_size) {
}

int execute_program(char* program_name) {
	return system(program_name);
}
