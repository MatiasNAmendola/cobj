%{
#include <stdlib.h>
#include <stdio.h>
#include "co.h"

#define YYSTYPE cnode

#include "co_parser.h"
#include "co_compile.h"

extern int fileno(FILE *);  /* avoids gcc warning */

#define CO_IS_OCT(c)    ((c)>='0' && (c)<='7')
#define CO_IS_HEX(c)    (((c)>='0' && (c)<='9') || ((c)>='a' && (c)<='f') || ((c)>='A' && (c)<='F'))

static void
co_scan_singlequoted_string(cval *cval)
{
    register char *s, *t;
    char *end;

    s = t = cval->u.str.val;
    end = s + cval->u.str.len;
    while (s < end) {
        if (*s == '\\') {
            s++;
            if (s >= end) {
                *t++ = '\\';
                break;
            }
            switch (*s) {
            case '\'':
                *t++ = '\'';
                cval->u.str.len--;
                break;
            case '\\':
                *t++ = *s;
                cval->u.str.len--;
                break;
            default:
                *t++ = '\\';
                *t++ = *s;
                break;
            }
        } else {
            *t++ = *s;
        }
        s++;
    }
    *t = 0;
}

static void
co_scan_doublequoted_string(cval *cval)
{
    register char *s, *t;
    char *end;

    s = t = cval->u.str.val;
    end = s + cval->u.str.len;
    while (s < end) {
        if (*s == '\\') {
            s++;
            if (s >= end) {
                *t++ = '\\';
                break;
            }
            switch (*s) {
            case 'a':
                *t++ = '\a';
                cval->u.str.len--;
                break;
            case 'b':
                *t++ = '\b';
                cval->u.str.len--;
                break;
            case 'f':
                *t++ = '\f';
                cval->u.str.len--;
                break;
            case 'n':
                *t++ = '\n';
                cval->u.str.len--;
                break;
            case 'r':
                *t++ = '\r';
                cval->u.str.len--;
                break;
            case 't':
                *t++ = '\t';
                cval->u.str.len--;
                break;
            case 'v':
                *t++ = '\v';
                cval->u.str.len--;
                break;
            case '"':
                *t++ = '"';
                cval->u.str.len--;
                break;
            case '\\':
                *t++ = *s;
                cval->u.str.len--;
                break;
            case 'x':
                if (CO_IS_HEX(*(s+1)) && CO_IS_HEX(*(s+2))) {
                    char hex_buf[3] = {0, 0, 0};
                    cval->u.str.len--; /* for the 'x' */

                    hex_buf[0] = *(++s);
                    cval->u.str.len--;
                    hex_buf[1] = *(++s);
                    cval->u.str.len--;
                    *t++ = (char)strtol(hex_buf, NULL, 16);
                } else {
                    *t++ = '\\';
                    *t++ = *s;
                }
                break;
            default:
                /* check for octal digits */
                if (CO_IS_OCT(*s)) {
                    char octal_buf[4] = {0, 0, 0, 0};
                    octal_buf[0] = *s;
                    cval->u.str.len--;
                    if (CO_IS_OCT(*(s+1))) {
                        octal_buf[1] = *(++s);
                        cval->u.str.len--;
                        if (CO_IS_OCT(*(s+1))) {
                            octal_buf[2] = *(++s);
                            cval->u.str.len--;
                        }
                    }
                    *t++ = (char)strtol(octal_buf, NULL, 8);
                } else {
                    *t++ = '\\';
                    *t++ = *s;
                }
                break;
            }
        } else {
            *t++ = *s;
        }
        s++;
    }
    *t = 0;
}

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
    yylval->u.val.type = CVAL_IS_INT;
    yylval->op_type = IS_CONST;
    return T_NUM;
}

{FNUM} {
    yylval->u.val.u.fval = strtof(yytext, NULL);
    yylval->u.val.type = CVAL_IS_FLOAT;
    yylval->op_type = IS_CONST;
    return T_FNUM;
}

"'".*"'" {
    yylval->u.val.u.str.val = xstrndup(yytext + 1, yyleng - 2);
    yylval->u.val.u.str.len = yyleng - 2;
    co_scan_singlequoted_string(&yylval->u.val);
    yylval->u.val.type = CVAL_IS_STRING;
    yylval->op_type = IS_CONST;
    return T_STRING;
}

"\"".*"\"" {
    yylval->u.val.u.str.val = xstrndup(yytext + 1, yyleng - 2);
    yylval->u.val.u.str.len = yyleng - 2;
    co_scan_doublequoted_string(&yylval->u.val);
    yylval->u.val.type = CVAL_IS_STRING;
    yylval->op_type = IS_CONST;
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
    yylval->u.val.type = CVAL_IS_STRING;
    yylval->op_type = IS_VAR;
    return T_NAME;
}

{TOKENS} {
    return *yytext;
}

"#".*   { /* ignore comments */ }

{FORMAT_SPACES}   { /* blank spaces which is used to format code are ignored */ }

.   {
    coerror("invalid chars");
}

%%
