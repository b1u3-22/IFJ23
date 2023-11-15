CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Wextra
LDFLAGS = -Wall -Wextra

all: scanner

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

token_stack.o: token_stack.h token_stack.c token.h
	$(CC) $(CFLAGS) -c -o $@ token_stack.c

