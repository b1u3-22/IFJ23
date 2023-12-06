CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Wextra
LDFLAGS = -Wall -Wextra

all: parser

run: all
	 ./parser

clean:
	rm -rf ./*.o

parser: parser.o token.o rule_stack.o scanner.o expression.o expression_stack.o token_stack.o analyzer.o symtable.o param_stack.o code_generator.o gen_stack.o
	$(CC) $(LDFLAGS) -o $@ $^ 

scanner: scanner.o token.o
	$(CC) $(LDFLAGS) -o $@ $^ 

scanner.o: scanner.c scanner.h token.h
	$(CC) $(CFLAGS) -c -o $@ scanner.c

token.o: token.h token.c
	$(CC) $(CFLAGS) -c -o $@ token.c

rule_stack.o: rule_stack.h rule_stack.c token.h
	$(CC) $(CFLAGS) -c -o $@ rule_stack.c

symtable.o: symtable.h symtable.c
	$(CC) $(CFLAGS) -c -o $@ symtable.c

parser.o: parser.h parser.c rule_stack.h token.h scanner.h expression.h expression_stack.h token_stack.h analyzer.h symtable.h param_stack.h code_generator.h
	$(CC) $(CFLAGS) -c -o $@ parser.c

expression.o: expression.h expression.c token.h scanner.h expression_stack.h code_generator.h gen_stack.h
	$(CC) $(CFLAGS) -c -o $@ expression.c

expression_stack.o: expression_stack.h expression_stack.c
	$(CC) $(CFLAGS) -c -o $@ expression_stack.c

token_stack.o: token_stack.h token_stack.c scanner.h token.h
	$(CC) $(CFLAGS) -c -o $@ token_stack.c

analyzer.o: analyzer.h analyzer.c token.h token_stack.h symtable.h param_stack.h
	$(CC) $(CFLAGS) -c -o $@ analyzer.c

param_stack.o: param_stack.h param_stack.c
	$(CC) $(CFLAGS) -c -o $@ param_stack.c

code_generator.o: code_generator.h code_generator.c expression.h
	$(CC) $(CFLAGS) -c -o $@ code_generator.c

gen_stack.o: gen_stack.h gen_stack.c token.h
	$(CC) $(CFLAGS) -c -o $@ gen_stack.c