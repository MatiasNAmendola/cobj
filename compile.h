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


COObject *compile(void);

// parser
int coparse();
void coerror(NodeList **xtop, const char *err, ...);

// scanner
int colex(YYSTYPE *colval);
int co_scanner_lex(YYSTYPE *yylval);
int co_scanner_setfile(COObject *f);
int co_scanner_setcode(char *code);

#endif
