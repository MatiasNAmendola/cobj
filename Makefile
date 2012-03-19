# The default target
all::

# Define CO_DEBUG=1 to debug (Defaults: 0)
# Define CC=clang to use clang to compile (Defaults: gcc)

CC = gcc
RM = rm -rf

CFLAGS = -std=c99 -Wall
ifdef CO_DEBUG
	CFLAGS += -DCO_DEBUG -g3
else
	CFLAGS += -O2
endif

LDFLAGS =

prefix = /usr/local
bindir = $(prefix)/bin

# recompile all lib objs if any of header file changes, dependencies is hard to maintain!
LIB_H = $(wildcard *.h) parser.h scanner.h $(wildcard objects/*.h)

LIB_OBJS += co.o
LIB_OBJS += serialize.o
LIB_OBJS += compile.o
LIB_OBJS += debug.o
LIB_OBJS += error.o
LIB_OBJS += llist.o
LIB_OBJS += object.o
LIB_OBJS += parser.o
LIB_OBJS += scanner.o
LIB_OBJS += stack.o
LIB_OBJS += usage.o
LIB_OBJS += vm_execute.o
LIB_OBJS += wrapper.o
LIB_OBJS += argparse/argparse.o
LIB_OBJS += dstring/dstring.o
LIB_OBJS += objects/typeobject.o
LIB_OBJS += objects/boolobject.o
LIB_OBJS += objects/intobject.o
LIB_OBJS += objects/floatobject.o
LIB_OBJS += objects/strobject.o
LIB_OBJS += objects/functionobject.o
LIB_OBJS += objects/codeobject.o
LIB_OBJS += objects/listobject.o
LIB_OBJS += objects/tupleobject.o
LIB_OBJS += objects/frameobject.o
LIB_OBJS += objects/dictobject.o
LIB_OBJS += objects/oplineobject.o

$(LIB_OBJS): $(LIB_H)

co: $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^ $(LIBS)

parser.h: parser.c
parser.c: parser.y
	bison -p co -v -d $^ -o $@

scanner.h: scanner.c
scanner.c: scanner.l
	re2c -cbdt scanner.h -o $@ $^

.PHONY: all install uninstall clean test indent tags doc bench

all:: co 

install: all
	install -p -m 755 co $(bindir)

uninstall: 
	$(RM) $(bindir)/co

clean:
	$(RM) co
	# library objects
	find . -name '*.[oa]' | xargs $(RM)
	# parser source files
	$(RM) parser.[ch]
	# scan source files
	$(RM) scanner.[ch]
	$(RM) *.output

doc:
	$(MAKE) -C doc/ html

test: all
	@$(MAKE) -C t/ all

bench:
	@cd bench; ./bench.sh

indent:
	@./indent.sh

tags:
	ctags -R --c-kinds=+p --fields=+S .
