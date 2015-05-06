#pragma once

/*
 * Vault will be written in friendly C. (C99 / C11 compatible)
 *
 * The program will use tab characters for indentation and #pragma directives to keep everything clean.
 * The program will follow K&R brace and indentation style.
 *
 * Naming style: GLOBAL_VALUE_CONSTANT_OR_MACRO, function_name, _internal_or_obscure_function, local_var, _private_struct_member, struct_member
 *
 * No global variables.
 */

#define VAULT_VERSION_STRING "0.1"

#include "platform.h"
#include "syscall.h"
#include "user.h"
#include "arg.h"
#include "wiz.h"
#include "file.h"
#include "sym_crypt.h"
#include "group.h"
