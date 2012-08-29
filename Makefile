# The default target
all::

# Define CO_DEBUG=1 to debug (Defaults: 0)
# Define YY_DEBUG=1 to debug re2c/bison scripts. (Defaults: 0)
# Define CC=clang to use clang to compile (Defaults: gcc)

CC = gcc
RM = rm -rf

CFLAGS = -std=c99 -Wall $(EXTRA_CFLAGS)
ifdef CO_DEBUG
	CFLAGS += -DCO_DEBUG -g3
else
	CFLAGS += -O2
endif

LDFLAGS =

prefix = /usr/local
bindir = $(prefix)/bin

FIND_SOURCE_FILES = find . \( -name .git -type d -prune \) -o \( -name '*.[hc]' -type f -print \)

# recompile all lib objs if any of header file changes, cuz dependencies is hard to maintain!
LIB_H = $(wildcard *.h) parser.h scanner.h $(wildcard objects/*.h) argparse/argparse.h

LIB_OBJS += cobj.o
LIB_OBJS += marshal.o
LIB_OBJS += compile.o
LIB_OBJS += error.o
LIB_OBJS += object.o
LIB_OBJS += parser.o
LIB_OBJS += scanner.o
LIB_OBJS += vm.o
LIB_OBJS += wrapper.o
LIB_OBJS += mem.o
LIB_OBJS += state.o
LIB_OBJS += ast.o
LIB_OBJS += opcode.o
LIB_OBJS += peephole.o
LIB_OBJS += arena.o
LIB_OBJS += builtin.o
LIB_OBJS += gc.o
LIB_OBJS += objects/typeobject.o
LIB_OBJS += objects/boolobject.o
LIB_OBJS += objects/noneobject.o
LIB_OBJS += objects/intobject.o
LIB_OBJS += objects/floatobject.o
LIB_OBJS += objects/strobject.o
LIB_OBJS += objects/functionobject.o
LIB_OBJS += objects/codeobject.o
LIB_OBJS += objects/listobject.o
LIB_OBJS += objects/listiterobject.o
LIB_OBJS += objects/tupleobject.o
LIB_OBJS += objects/frameobject.o
LIB_OBJS += objects/dictobject.o
LIB_OBJS += objects/bytesobject.o
LIB_OBJS += objects/fileobject.o
LIB_OBJS += objects/exceptionobject.o
LIB_OBJS += objects/capsuleobject.o
LIB_OBJS += objects/cfunctionobject.o
LIB_OBJS += objects/cellobject.o
LIB_OBJS += objects/rangeobject.o
LIB_OBJS += objects/rangeiterobject.o
LIB_OBJS += objects/setobject.o

LIB_OBJS += linenoise/linenoise.o
LIB_OBJS += argparse/argparse.o

$(LIB_OBJS): $(LIB_H)

cobj: $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^ $(LIBS)

argparse/argparse.h:
	@if test ! -f argparse/argparse.c; then \
		git submodule update --init --recursive; \
	fi;

parser.h: parser.c
parser.c: parser.y
	bison -p co -v -d $^ -o $@

scanner.h: scanner.c
scanner.c: scanner.l
	re2c -cbdt scanner.h -o $@ $^

.PHONY: all install uninstall clean test indent tags doc bench

all:: cobj

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

analyzer: parser.h scanner.h
	find . -name '*.[c]' | xargs clang -cc1 -analyze

doc:
	$(MAKE) -C doc/ html

test: all
	@$(MAKE) -C t/ all

bench:
	@cd bench; ./bench.sh

indent:
	@./indent.sh

tags:
	$(RM) tags
	$(FIND_SOURCE_FILES) | xargs ctags -a
