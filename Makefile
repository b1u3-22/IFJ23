CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Wextra
LDFLAGS = -Wall -Wextra

all: scanner clean

run: all
	./scanner

clean:
	rm -rf ./*.o

scanner: scanner.o token.o
	$(CC) $(LDFLAGS) -o $@ $^ 

scanner.o: scanner.c scanner.h token.h
	$(CC) $(CFLAGS) -c -o $@ scanner.c

token.o: token.h token.c
	$(CC) $(CFLAGS) -c -o $@ token.c
