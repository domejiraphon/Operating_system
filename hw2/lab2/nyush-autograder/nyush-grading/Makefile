CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -Wall -Werror -Wextra

.PHONY: all
all: nyush

nyush: nyush.o utils.o

nyush.o: nyush.c utils.h

utils.o : utils.c utils.h

.PHONY: clean
clean:
	rm -f *.o nyush