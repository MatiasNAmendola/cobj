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

// block
struct block {
    /* Each block in compiler unit is linked via b_listnext in the reverse
     * order. */
    struct block *b_listnext;
    struct instr *b_instr;  /* pointer to an array of instructions */
    int b_iused;    /* number of instructions used */
    int b_ialloc;   /* length of instruction array */
    /* If b_next is non-NULL, it's a pointer to the next block reached by normal
     * control flow. */
    struct block *b_next;
    unsigned b_seen : 1;    /* flag to used to perform a DFS of blocks */
    unsigned b_return : 1;  /* flag that indicates a RETURN opcode is inserted */
    int b_stackdepth;       /* depth of stack upon entry of block, computed by stackdepth() */
};

/* 
 * Compliation unit, change on entry and exit of function scope.
 */
struct compiler_unit {
    COObject *bytecode;
    COObject *consts;
    COObject *names;

    int bytecode_offset;

    /* Pointer to the most recently allocated block. By following b_listnext,
     * you can reach all early allocated blocks. */
    struct block *u_block;

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
