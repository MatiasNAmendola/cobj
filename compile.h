#ifndef COMPILE_H
#define COMPILE_H

#include "compat.h"
#include "object.h"
#include "objects/listobject.h"
#include "ast.h"
#include "arena.h"

/* Change whenever the bytecode emmited by the compiler may no longer be
 * understood by old code evaluator.
 */
#define CODEDUMP_MAGIC  (314 << 16 | 'c' << 8 | 'o')

COObject *compile(struct arena *arena);

struct compiler_unit;

struct compiler {
    Node *xtop;

    struct compiler_unit *u;    /* Current compiler unit. */
    COObject *stack;            /* List object to hold compiler unit pointers. */
    int nestlevel;
    struct arena *arena;
};

// parser
#include "parser.h"
int coerror(struct compiler *c, const char *err, ...);

// scanner
int colex(YYSTYPE *colval);
int scanner_lex(YYSTYPE *yylval);
void scanner_init(struct arena *arena);
int scanner_setcode(char *code);

#endif
