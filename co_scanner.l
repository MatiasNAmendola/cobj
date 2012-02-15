#include "co.h"
#include "co_compile.h"
#include "co_scanner.h"
#include "co_parser.h"

struct co_scanner_globals {
    FILE *yy_in;
    FILE *yy_out;

    unsigned int yy_leng;
    unsigned char *yy_start;
    unsigned char *yy_text;
    unsigned char *yy_cursor;
    unsigned char *yy_marker;
    unsigned char *yy_limit;
    int yy_state;
    co_stack state_stack;
};

struct co_scanner_globals scanner_globals;

/*!max:re2c*/

/*** S re2c interface code ***/
#define YYCTYPE     unsigned char
#define YYFILL(n)   { if ((YYCURSOR + n) >= (YYLIMIT + YYMAXFILL)) { return 0; }}
#define YYCURSOR    scanner_globals.yy_cursor
#define YYLIMIT     scanner_globals.yy_limit
#define YYMARKER    scanner_globals.yy_marker
#ifdef CO_DEBUG
# define YYDEBUG(s, c)   printf("state: %d char: %c\n", s, c)
#else
# define YYDEBUG(s, c)
#endif
/*** E re2c interface code ***/

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

void
co_scanner_startup()
{
    co_stack_init(&scanner_globals.state_stack);
}

void
co_scanner_shutdown()
{
    co_stack_destory(&scanner_globals.state_stack);
}

int
co_scanner_openfile(FILE *in)
{
    scanner_globals.yy_in = in;
    scanner_globals.yy_start = NULL;
    
}

int
co_scanner_lex(cval *cval)
{
/*!re2c
re2c:yyfill:check = 0;

NUM     [0-9]+
FNUM    ([0-9]*"."[0-9]+)
HNUM    "0x"[0-9a-fA-F]+
NAME    [a-zA-Z_][a-zA-Z0-9_]*
TOKENS  [-+()=>/<*^,;:{}%]
WHITESPACE   [ \t\n]+
ANY_CHAR [^]

<SC_INIT>{NUM} {
    // TODO
}

<SC_INIT>{FNUM} {
    // TODO
}

<SC_INIT>"if" {
    return T_IF;
}

<SC_INIT>"else" {
    return T_ELSE;
}

<SC_INIT>"while" {
    return T_WHILE;
}

<SC_INIT>"func" {
    return T_FUNC;
}

<SC_INIT>"print" {
    return T_PRINT;
}

<SC_INIT>"None" {
    // TODO
}

<SC_INIT>"True" {
    // TODO
}

<SC_INIT>"False" {
    // TODO
}

<SC_INIT>{NAME} {
    // TODO
}

<SC_INIT>{TOKENS} {
}

<SC_INIT>"#".*   { /* ignore comments */ }

<SC_INIT>{WHITESPACE}   { /* blank spaces which is used to format code are ignored */ }

<SC_INIT>{ANY_CHAR}   {
    // TODO
}
*/
}
