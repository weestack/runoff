# Specify what source C files we have
SOURCES=lexer.c parser.c runoff.c ast.c prettyprint.c symbol.c typechecker.c buildsymboltable.c auxiliary.c codegen.c removeNestedDecls.c constraints.c checkused.c initializeVars.c

# What headers do we have?
HEADERS=phases.h data.h auxiliary.h

# The objects are just the sources with .c swapped out for .o
OBJECTS=$(SOURCES:.c=.o)

# The following is a list of machine generated files, which are removed
# when running 'make clean'
GENFILES=parser.c parser.h lexer.c

# What is the name of our binary?
PROG=runoff

# What folder are we compiling from?
COMPILEDIR=$(shell pwd)

# Specify the C flags and linker flags.
# We want ansi c with all warnings
# We want debug info
CFLAGS=-std=c89 -pedantic -Wall -Wextra -Werror -g -DCOMPILEDIR="\"$(COMPILEDIR)\""

# We want to link with the flex library
LDFLAGS=-lfl

# Rule for combining all the objects into our binary
$(PROG): $(OBJECTS)
	cc $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o $(PROG)
	@echo [CC] $(OBJECTS)
	@echo "There you go $(shell whoami), I have compiled $(PROG) for you :)"

# How to generate parser.c from our yacc file
parser.c: runoff.yacc $(HEADERS)
	@bison --defines=parser.h -o parser.c -t runoff.yacc
	@echo [BISON] $<

# parser.h is generated at the same time as parser.c so
# this empty rule will force make to generate parser.c if
# parser.h doesn't exist
parser.h: parser.c

# Generate lexer.c from our lex file
lexer.c: runoff.lex $(HEADERS)
	@flex -o lexer.c runoff.lex
	@echo [FLEX] $<

# Compile lexer.c into lexer.o
lexer.o: lexer.c parser.h
	@cc -c lexer.c
	@echo [CC] lexer.c

# Compile parser.c into parer.h
parser.o: parser.c parser.h
	@cc -c parser.c
	@echo [CC] parser.c

# Generic rule to compile c files into o files using our CFLAGS
%.o: %.c $(HEADERS)
	@cc $(CFLAGS) -c $<
	@echo [CC] $<

.PHONY: clean test
# Clean just removes the generated filed, the objects, and the binary
clean:
	@rm -rf $(GENFILES) $(OBJECTS) $(PROG)
	@echo [RM] $(GENFILES) $(OBJECTS) $(PROG)

test: $(PROG)
	@./tests/runner.sh