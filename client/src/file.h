#pragma once

#include "types.h"

/* This file just allows us to pull file data really quickly into program memory. */
/* Memory should be freed when use is finished. */

vault_buffer vault_file_read(char* filename, ...);

int vault_file_write(char* filename, char* fmt,  ...);
int vault_file_write_raw(char* filename, char* data, int size);
