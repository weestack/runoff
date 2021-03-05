SOURCES=lexer.c parser.c runoff.c
OBJECTS=$(SOURCES:.c=.o)
GENFILES=parser.c parser.h lexer.c

PROG=runoff

CFLAGS=-ansi -pedantic -Wall -Werror
LDFLAGS=-lfl

$(PROG): $(OBJECTS)
	cc $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o $(PROG)

parser.c: runoff.yacc
	bison -d -t runoff.yacc
	mv runoff.tab.cacc parser.c
	mv runoff.tab.hacc parser.h

parser.h: parser.c

lexer.c: runoff.lex parser.h
	flex runoff.lex
	mv lex.yy.c lexer.c

lexer.o: lexer.c parser.h
	cc -c lexer.c

parser.o: parser.c parser.h
	cc -c parser.c

%.o: %.c
	cc $(CFLAGS) -c $<

clean:
	rm -rf $(GENFILES) $(OBJECTS) $(PROG)
