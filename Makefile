# The default target
all::

CC = gcc
RM = rm -rf

CFLAGS = -std=c99 -O2 -Wall
LDFLAGS =

prefix = /usr/local
bindir = $(prefix)/bin

LIB_H += co.h
LIB_H += co_compile.h
LIB_H += co_config.h
LIB_H += co_hash.h
LIB_H += co_node.h
LIB_H += co_parser.h
LIB_H += co_scanner.h
LIB_H += co_stack.h
LIB_H += co_vm_execute.h
LIB_H += co_operators.h
LIB_H += co_llist.h

LIB_OBJS += co.o
LIB_OBJS += co_alloc.o
LIB_OBJS += co_compile.o
LIB_OBJS += co_config.o
LIB_OBJS += co_hash.o
LIB_OBJS += co_node.o
LIB_OBJS += co_parser.o
LIB_OBJS += co_scanner.o
LIB_OBJS += co_stack.o
LIB_OBJS += co_usage.o
LIB_OBJS += co_vm_execute.o
LIB_OBJS += co_wrapper.o
LIB_OBJS += co_operators.o
LIB_OBJS += co_llist.o

LIBS =

local.h:
	./genlocal.sh

$(LIB_OBJS): $(LIB_H) local.h

co: $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^ $(LIBS)

co_parser.h: co_parser.c
co_parser.c: co_parser.y
	bison -d $^ -o $@

co_scanner.h: co_scanner.c
co_scanner.c: co_scanner.l
	flex --header-file=co_scanner.h -o $@ $^


.PHONY: all install uninstall clean test indent tags

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

test: all
	./t/test.co

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
