CC = g++
CFLAGS = -std=c++14 -Wall -g
LDFLAGS = -lcpprest -lazurestorage

OUTPUT = vault_upload
SOURCES = vault_upload.cpp vault_azure.cpp

OBJECTS = $(SOURCES:.cpp=.o)
VPATH = src

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(OUTPUT)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS)
