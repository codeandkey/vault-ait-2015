#pragma once

/* This file just allows us to pull file data really quickly into program memory. */
/* Memory should be freed when use is finished. */

char* vault_file_read(char* filename, ...);
char* vault_file_read_size(int* file_size, char* filename, ...);

char* vault_file_read_home(char* filename, ...);

int vault_file_write_home(char* filename, char* fmt,  ...);
int vault_file_write_home_raw(char* filename, char* data, int size);
