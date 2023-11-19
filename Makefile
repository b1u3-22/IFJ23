CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Wextra
LDFLAGS = -Wall -Wextra

all: scanner parser

run: all
	./scanner

clean:
	rm -rf ./*.o

parser: parser.o token.o token_stack.o scanner.o
	$(CC) $(LDFLAGS) -o $@ $^ 

scanner: scanner.o token.o
	$(CC) $(LDFLAGS) -o $@ $^ 

scanner.o: scanner.c scanner.h token.h
	$(CC) $(CFLAGS) -c -o $@ scanner.c

token.o: token.h token.c
	$(CC) $(CFLAGS) -c -o $@ token.c

token_stack.o: token_stack.h token_stack.c token.h
	$(CC) $(CFLAGS) -c -o $@ token_stack.c

symtable.o: symtable.h symtable.c
	$(CC) $(CFLAGS) -c -o $@ symtable.c

parser.o: parser.h parser.c token_stack.h token.h scanner.h
	$(CC) $(CFLAGS) -c -o $@ parser.c