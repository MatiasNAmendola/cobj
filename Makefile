# The default target
all::

# Define CO_DEBUG=1 to debug (Defaults: 0)
# Define CC=clang to use clang to compile (Defaults: gcc)

CC = gcc
RM = rm -rf

CFLAGS = -std=c99 -O2 -Wall
ifdef CO_DEBUG
	CFLAGS += -DCO_DEBUG
endif

LDFLAGS =

prefix = /usr/local
bindir = $(prefix)/bin

LIB_H = $(wildcard *.h) local.h co_parser.h co_scanner.h

LIB_OBJS += co.o
LIB_OBJS += co_compile.o
LIB_OBJS += co_debug.o
LIB_OBJS += co_hash.o
LIB_OBJS += co_parser.o
LIB_OBJS += co_scanner.o
LIB_OBJS += co_stack.o
LIB_OBJS += co_usage.o
LIB_OBJS += co_vm_execute.o
LIB_OBJS += co_vm_opcodes.o
LIB_OBJS += co_wrapper.o
LIB_OBJS += co_llist.o

local.h:
	./genlocal.sh

$(LIB_OBJS): $(LIB_H)

co: $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^ $(LIBS)

co_parser.h: co_parser.c
co_parser.c: co_parser.y
	bison -p co -v -d $^ -o $@

co_scanner.h: co_scanner.c
co_scanner.c: co_scanner.l
	re2c -cbdFt co_scanner.h -o $@ $^

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
	$(RM) $(LIB_OBJS)
	# parser source files
	$(RM) co_parser.[ch]
	# scan source files
	$(RM) co_scanner.[ch]
	# local header
	$(RM) local.h

doc:
	$(MAKE) -C doc/ html

test: all
	./t/basic.co
	./t/literals.co
	./t/ifelse.co
	./t/function.co
	./t/while.co

indent:
	@# try to find out all typenames defined by 'typedef' of c
	@test -e ~/.indent.pro && cp ~/.indent.pro .indent.pro
	@sed -n 's/.*typedef\s.*\s\([a-zA-Z_]\+\);/\1/p' *.[ch] | xargs \
		-Itype echo -T type >> .indent.pro
	@indent *.[ch]
	@$(RM) .indent.pro
	@$(RM) *~

tags:
	ctags -R --c-kinds=+p --fields=+S .
