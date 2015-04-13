CC = g++
CFLAGS = -std=c++11 -Wall -pedantic
LDFLAGS = -lcpprest -lazurestorage

SOURCES = io.cpp interface.cpp azure.cpp
OBJECTS = $(SOURCES:.cpp=.o)

OUTPUT = vaultio
VPATH = src

all: $(OUTPUT)

$(OUTPUT): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(OUTPUT)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS)
