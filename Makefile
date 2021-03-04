SOURCES=lexer.c parser.c runoff.c
OBJECTS=$(SOURCES:.c=.o)
GENFILES=parser.c parser.h lexer.c

PROG=runoff

$(PROG): $(OBJECTS)
	cc $(OBJECTS) -lfl -o $(PROG)

parser.c: runoff.yacc
	bison -d -t runoff.yacc
	mv runoff.tab.cacc parser.c
	mv runoff.tab.hacc parser.h

parser.h: parser.c

lexer.c: runoff.lex parser.h
	flex runoff.lex
	mv lex.yy.c lexer.c

%.o: %.c
	cc -c $<

clean:
	rm -rf $(GENFILES) $(OBJECTS) $(PROG)