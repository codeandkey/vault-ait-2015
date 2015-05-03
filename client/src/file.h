#pragma once

/* This file just allows us to pull file data really quickly into program memory. */
/* Memory should be freed when use is finished. */

char* vault_file_read(char* filename);
char* vault_file_read_size(char* filename, int* file_size);
