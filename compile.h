#ifndef COMPILE_H
#define COMPILE_H

#include "co_compat.h"
#include "object.h"
#include "objects/listobject.h"
#include "ast.h"
#include "parser.h"

/* Change whenever the bytecode emmited by the compiler may no longer be
 * understood by old code evaluator.
 */
#define CODEDUMP_MAGIC  (314 << 16 | 'c' << 8 | 'o')

/** cnode type **/
#define IS_CONST        (1<<0)
#define IS_TMP_VAR      (1<<1)
#define IS_VAR          (1<<2)
#define IS_UNUSED       (1<<3)

struct cnode {
    int type;
    union {
        COObject *co;
        uint var;
        int opline_num;
    } u;
};

// compiler
struct instr {
    unsigned int i_hasarg : 1;
    unsigned char i_opcode;
    int i_oparg;
};

struct compiler {
    /* For Code object */
    COObject *bytecode;
    COObject *consts;
    COObject *names;
    /* ! For Code Object */

    int bytecode_offset;

    /* AST */
    NodeList  *xtop;
    /* ! AST */

    struct instr *instr;  /* pointer to an array of instructions */
    int iused;            /* number of instructions used */
    int ialloc;           /* length of instructions array */
};

COObject *co_compile(void);

// parser
int coparse();
void coerror(struct compiler *c, const char *err, ...);

// scanner
int colex(YYSTYPE *colval);
int co_scanner_lex(YYSTYPE *yylval);
int co_scanner_setfile(COObject *f);
int co_scanner_setcode(char *code);

#endif
