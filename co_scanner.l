%{
#include <stdlib.h>
#include <stdio.h>
#include "co.h"

#define YYSTYPE cnode

#include "co_parser.h"
#include "co_compile.h"

extern int fileno(FILE *);  /* avoids gcc warning */
%}

%option bison-bridge
%option reentrant
%option noyywrap

NUM     [0-9]+
FNUM    ([0-9]*"."[0-9]+)
HNUM    "0x"[0-9a-fA-F]+
NAME    [a-zA-Z_\x7f-\xff][a-zA-Z0-9_\x7f-\xff]*
TOKENS  [-+()=/*^,;:{}%]
FORMAT_SPACES   [ \t\n]

%%

{NUM} {
    if (yyleng < MAX_LENGTH_OF_LONG - 1) { /* Won't overflow */
        yylval->u.val.u.ival = strtol(yytext, NULL, 0);
    } else { /* Maybe overflow */
        errno = 0;
        yylval->u.val.u.ival = strtol(yytext, NULL, 0);
    }
    yylval->op_type = T_NUM;
    return T_NUM;
}

{FNUM} {
    yylval->u.val.u.fval = strtof(yytext, NULL);
    yylval->op_type = T_FNUM;
    return T_FNUM;
}


"\""[^"]*"\"" {
    yylval->u.val.u.str.val = xstrndup(yytext + 1, yyleng - 2);
    yylval->u.val.u.str.len = yyleng;
    yylval->op_type = T_STRING;
    return T_STRING;
}

"if"    {
    return T_IF;
}

"else"  {
    return T_ELSE;
}

"func" {
    return T_FUNC;
}

"print" {
    return T_PRINT;
}

{NAME} {
    yylval->u.val.u.str.val = xstrndup(yytext, yyleng);
    yylval->u.val.u.str.len = yyleng;
    yylval->op_type = T_NAME;
    return T_NAME;
}

{TOKENS} {
    return *yytext;
}

"#".*   { /* ignore comments */}

{FORMAT_SPACES}   { /* blank spaces which is used to format code are ignored */}

.   {
    coerror("invalid chars");
}

%%
