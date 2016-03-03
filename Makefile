CC=g++
CFLAGS=-Wall -g
SOURCES=clipdaemon.cpp
EXECUTABLE=clipd

all:
	mkdir -p obj
	$(CC) $(CFLAGS) $(SOURCES) -lX11 -o obj/${EXECUTABLE}

clean:
	rm -rf obj/*

