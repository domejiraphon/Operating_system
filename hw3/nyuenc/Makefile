CC=gcc
CFLAGS=-g -pedantic -std=gnu17 -fsigned-char -Wall -Werror -Wextra -O2
LDFLAGS=-pthread 
.PHONY: all
all: nyuenc

nyuenc: nyuenc.o

nyuenc.o: nyuenc.c

.PHONY: clean
clean:
	rm -f *.o nyuenc
