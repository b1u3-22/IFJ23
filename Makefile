CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Wextra
LDFLAGS = -Wall -Wextra

all: scanner clean

run: all
	./scanner

clean:
	rm -rf ./*.o

scanner: scanner.o
	$(CC) $(LDFLAGS) -o $@ $^ 

scanner.o: scanner.c scanner.h global_structs.h
	$(CC) $(CFLAGS) -c -o $@ scanner.c
