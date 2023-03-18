CC=gcc
CFLAGS=-g -pedantic -pthread -std=gnu17

.PHONY: all
all: nyuenc

nyuenc: nyuenc.o utils.o

nyuenc.o: nyuenc.c utils.h

utils.o : utils.c utils.h

.PHONY: clean
clean:
	rm -f *.o nyuec
