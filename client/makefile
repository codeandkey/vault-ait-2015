SOURCES = vault.c syscall.c user.c hash.c wiz.c arg.c file.c sym_crypt.c util.c

VPATH = src

LINUX_CC = gcc
LINUX_CFLAGS = -std=gnu11 -Wall -fdiagnostics-color=auto -DVAULT_BUILD_DEBUG -g
LINUX_LDFLAGS = -lcrypto -lgcrypt -lssl

LINUX_OUTPUT = vault
LINUX_OBJECTS = $(SOURCES:.c=.o)

WIN32_CC = x86_64-w64-mingw32-gcc
WIN32_CFLAGS = -std=c99 -Wall
WIN32_LDFLAGS = -lcrypto -lgcrypt -lssl

WIN32_OUTPUT = vault.exe
WIN32_OBJECTS = $(SOURCES:.c=.wo)

OSX_CC = echo 'OSX toolchain not set up!' #
OSX_CFLAGS = -std=c99 -Wall
OSX_LDFLAGS = -lcrypto -lgcrypt -lssl

OSX_OUTPUT = vault_osx
OSX_OBJECTS = $(SOURCES:.c=.xo)

all: $(LINUX_OUTPUT) $(WIN32_OUTPUT) $(OSX_OUTPUT)
	@printf '[vault] \e[0;35mFinished compilation successfully!\e[0;39m\n'

linux: $(LINUX_OUTPUT)
win32: $(WIN32_OUTPUT)
osx: $(OSX_OUTPUT)

$(LINUX_OUTPUT): $(LINUX_OBJECTS)
	@$(LINUX_CC) $(LINUX_OBJECTS) $(LINUX_LDFLAGS) -o $(LINUX_OUTPUT)
	@printf '[linux] \e[0;36mDone.\e[0;39m\n'

$(WIN32_OUTPUT): $(WIN32_OBJECTS)
	@$(WIN32_CC) $(WIN32_OBJECTS) $(WIN32_LDFLAGS) -o $(WIN32_OUTPUT)
	@printf '[win32] \e[0;32mDone.\e[0;39m\n'

$(OSX_OUTPUT):
	@printf '[osx]   \e[0;31mThe OSX toolchain is not set up yet! Compiles only for Win32 and Linux platforms for now.\e[0;39m\n'
	@printf '[osx]   \e[0;33mDone.\e[0;39m\n'

%.o: %.c
	@printf '[linux] \e[0;36mCompiling %s ..\e[0;39m\n' $<
	@$(LINUX_CC) $(LINUX_CFLAGS) -c $< -o $@

%.wo: %.c
	@printf '[win32] \e[0;32mCompiling %s ..\e[0;39m\n' $<
	@$(WIN32_CC) $(WIN32_CFLAGS) -c $< -o $@

%.xo: %.c
	@printf '[osx]   \e[0;33mCompiling %s ..\e[0;39m\n' $<
	@$(OSX_CC) $(OSX_CFLAGS) -c $< -o $@

clean:
	@rm -f $(LINUX_OBJECTS) $(WIN32_OBJECTS) $(OSX_OBJECTS)

cleanbin: clean
	@rm -f $(LINUX_OUTPUT) $(WIN32_OUTPUT) $(OSX_OUTPUT)
