# The default target
all::

# Define CO_DEBUG=1 to debug (Defaults: 0)
# Define CC=clang to use clang to compile (Defaults: gcc)

CC = gcc
RM = rm -rf

CFLAGS = -std=c99 -gdwarf-2 -g3 -Wall
ifdef CO_DEBUG
	CFLAGS += -DCO_DEBUG
else
	CFLAGS += -O2 
endif

LDFLAGS =

prefix = /usr/local
bindir = $(prefix)/bin

LIB_H = $(wildcard *.h) parser.h scanner.h

LIB_OBJS += co.o
LIB_OBJS += compile.o
LIB_OBJS += debug.o
LIB_OBJS += hash.o
LIB_OBJS += error.o
LIB_OBJS += parser.o
LIB_OBJS += scanner.o
LIB_OBJS += stack.o
LIB_OBJS += vm_execute.o
LIB_OBJS += vm_opcodes.o
LIB_OBJS += wrapper.o
LIB_OBJS += usage.o
LIB_OBJS += llist.o
LIB_OBJS += dstring.o

$(LIB_OBJS): $(LIB_H)

co: $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^ $(LIBS)

parser.h: parser.c
parser.c: parser.y
	bison -p co -v -d $^ -o $@

scanner.h: scanner.c
scanner.c: scanner.l
	re2c -cbdt scanner.h -o $@ $^

.PHONY: all install uninstall clean test indent tags doc

all:: co

install: all
	install -m 755 co $(bindir)

uninstall: 
	$(RM) $(bindir)/co

clean:
	$(RM) co
	# indent backup files
	$(RM) *~
	# library objects
	$(RM) *.o
	# parser source files
	$(RM) parser.[ch]
	# scan source files
	$(RM) scanner.[ch]
	$(RM) *.output

doc:
	$(MAKE) -C doc/ html

test: all
	@$(MAKE) -C t/ all

indent:
	@./indent.sh

tags:
	ctags -R --c-kinds=+p --fields=+S .
