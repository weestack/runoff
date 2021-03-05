# todo comment whats happening in this file
SOURCES=lexer.c parser.c runoff.c
OBJECTS=$(SOURCES:.c=.o)
GENFILES=parser.c parser.h lexer.c

PROG=runoff

CFLAGS=-ansi -pedantic -Wall -Wextra -Werror
LDFLAGS=-lfl

$(PROG): $(OBJECTS)
	cc $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o $(PROG)

parser.c: runoff.yacc
	bison --defines=parser.h -o parser.c -t runoff.yacc

parser.h: parser.c

lexer.c: runoff.lex parser.h
	flex -o lexer.c runoff.lex

lexer.o: lexer.c parser.h
	cc -c lexer.c

parser.o: parser.c parser.h
	cc -c parser.c

%.o: %.c
	cc $(CFLAGS) -c $<

clean:
	rm -rf $(GENFILES) $(OBJECTS) $(PROG)
