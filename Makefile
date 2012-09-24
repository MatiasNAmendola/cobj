# The default target
all::

# Define CO_DEBUG=1 to debug (Defaults: 0)
# Define YY_DEBUG=1 to debug re2c/bison scripts. (Defaults: 0)
# Define CC=clang to use clang to compile (Defaults: gcc)
# Define CO_DEBUG_FRAME=1 to debug frame object stuff (Defaults: 0)

CC = gcc
RM = rm -rf

EXTRA_CFLAGS =  # dynamically added/removed
CFLAGS = -std=c99 -pedantic -Wall $(EXTRA_CFLAGS)
ifdef CO_DEBUG
	CFLAGS += -DCO_DEBUG -g3
else
	CFLAGS += -O2
endif

LDFLAGS = -lm

PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

FIND_SOURCE_FILES = find . \( -name .git -type d -prune \) -o \( -name '*.[hc]' -type f -print \)

# Recompile all lib objs if any of header file changes, cuz dependencies is hard to maintain!
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
LIB_OBJS += gc.o
LIB_OBJS += str2d.o
LIB_OBJS += $(patsubst %.c,%.o,$(wildcard objects/*.c))
LIB_OBJS += module.o
LIB_OBJS += $(patsubst %.c,%.o,$(wildcard modules/*.c))

LIB_OBJS += linenoise/linenoise.o
LIB_OBJS += argparse/argparse.o

linenoise/linenoise.o:
	@make -C linenoise linenoise.o

argparse/argparse.o:
	@make -C argparse argparse.o

$(LIB_OBJS): $(LIB_H)

version.h: gen-version.sh
	./gen-version.sh > version.h

cobj: $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^ $(LIBS)

cobj.o: version.h

argparse/argparse.h linenoise/linenoise.h:
	@if test ! -f argparse/argparse.c; then \
		git submodule update --init --recursive; \
	fi;

parser.h: parser.c
parser.c: parser.y
	bison -p co -v -d $^ -o $@

scanner.h: scanner.c
scanner.c: scanner.l
	re2c -cbdFt scanner.h -o $@ $^

.PHONY: all install uninstall clean test tags doc

all:: cobj

install: all
	install -p -m 755 cobj $(BINDIR)

uninstall:
	$(RM) $(BINDIR)/cobj

clean:
	$(RM) cobj
	find . -name '*.[oa]' | xargs $(RM)
	$(RM) *.output

analyzer: parser.h scanner.h
	find . -name '*.[c]' | xargs clang -cc1 -analyze

doc:
	$(MAKE) -C doc/ html

test: all
	$(MAKE) -C t/ all

newtest: all
	$(MAKE) -C test/ all

tags:
	$(RM) tags
	$(FIND_SOURCE_FILES) | xargs ctags -a
