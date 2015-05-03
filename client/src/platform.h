#pragma once

#if defined(_WIN32)
#define VAULT_PLATFORM_WIN32
#define VAULT_PLATFORM_STRING "Win32"
#elif defined(__APPLE__)
#define VAULT_PLATFORM_OSX
#define VAULT_PLATFORM_OSX "OSX"
#elif defined(__linux)
#define VAULT_PLATFORM_LINUX
#define VAULT_PLATFORM_STRING "Linux"
#else
#error Vault: No supported platform macros detected! Supported: Linux, Win32 (64), OSX
#endif
