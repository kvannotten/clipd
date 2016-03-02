CC=g++
CFLAGS=-c -Wall
SOURCES=clipdaemon.cpp
EXECUTABLE=clipd

all:
	mkdir -p obj
	$(CC) $(CFLAGS) $(SOURCES) -lX11 -o obj/${EXECUTABLE}

clean:
	rm -rf obj/*

