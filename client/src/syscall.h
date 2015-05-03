#pragma once

#include <stdarg.h>

/* This defines the abstraction for cross-platform program calling. */

int vault_syscall(const char* program_fmt, ...);
