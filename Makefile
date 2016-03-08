CC=gcc
CFLAGS=-Wall -g
SOURCES=clipd.c
EXECUTABLE=clipd

all:
	mkdir -p obj
	$(CC) $(CFLAGS) $(SOURCES) -lX11 -o obj/${EXECUTABLE}

clean:
	rm -rf obj/*

