#ifndef COMPILE_H
#define COMPILE_H

#include "compat.h"
#include "object.h"
#include "objects/listobject.h"
#include "ast.h"
#include "parser.h"

/* Change whenever the bytecode emmited by the compiler may no longer be
 * understood by old code evaluator.
 */
#define CODEDUMP_MAGIC  (314 << 16 | 'c' << 8 | 'o')

// instruction
struct instr {
    unsigned int i_hasarg:1;
    unsigned char i_opcode;
    int i_oparg;
};

/* 
 * Compliation unit, change on entry of exit of function scope.
 */
struct compiler_unit {
    COObject *bytecode;
    COObject *consts;
    COObject *names;

    int bytecode_offset;
    struct instr *instr;        /* pointer to an array of instructions */
    int iused;                  /* number of instructions used */
    int ialloc;                 /* length of instructions array */

    int argcount;
};

struct compiler {
    NodeList *xtop;

    struct compiler_unit *u;    /* Current compiler unit. */
    COObject *stack;            /* List object to hold compiler unit pointers. */
    int nestlevel;
};

COObject *compile(void);

// parser
int coparse();
void coerror(struct compiler *c, const char *err, ...);

// scanner
int colex(YYSTYPE *colval);
int co_scanner_lex(YYSTYPE *yylval);
int co_scanner_setfile(COObject *f);
int co_scanner_setcode(char *code);

#endif
