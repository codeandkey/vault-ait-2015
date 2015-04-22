#ifndef VAULT_PLATFORM_H
#define VAULT_PLATFORM_H

#ifdef __linux__
#define VAULT_PLATFORM_LINUX
#elif defined (__MINGW32__)
#define VAULT_PLATFORM_WINDOWS
#elif defined (__APPLE__)
#define VAULT_PLATFORM_APPLE
#endif

#endif
