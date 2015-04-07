CC = gcc
CFLAGS = -std=c99 -Wall
LDFLAGS = -lstdc++

OUTPUT = vault
VPATH = src

SOURCES = vault.c vault_cli.c

OBJECTS = $(SOURCES:.c=.o)

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(OUTPUT)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) vault.log
