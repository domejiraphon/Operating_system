CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -fsigned-char
LDFLAGS=-lcrypto

.PHONY: all
all: nyufile

nyufile: nyufile.o utils.o

nyufile.o: nyufile.c utils.h

utils.o : utils.c

.PHONY: clean
clean:
	rm -f *.o nyufile
