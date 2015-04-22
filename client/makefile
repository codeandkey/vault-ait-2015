CC = gcc
CFLAGS = -std=c99 -Wall -fdiagnostics-color=always
LDFLAGS = -lssl -lcrypto -lgcrypt

OUTPUT = vault
VPATH = src

SOURCES = vault.c vault_cli.c vault_conf.c vault_aes.c vault_hash.c vault_encrypt.c

OBJECTS = $(SOURCES:.c=.o)

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(OUTPUT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) vault.log
