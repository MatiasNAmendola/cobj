#include "co.h"
#include "compile.h"
#include "scanner.h"
#include "parser.h"
#include "dstring.h"

struct co_scanner_globals {
    int yy_in;

    unsigned int yy_leng;
    char *yy_start;
    char *yy_text;
    char *yy_cursor;
    char *yy_marker;
    char *yy_limit;
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
#define YYGETCONDITION()  scanner_globals.yy_state
#define YYSETCONDITION(s) scanner_globals.yy_state = s
#ifdef CO_DEBUG
# define YYDEBUG(s, c)   printf("state: %d char: %c\n", s, c)
#else
# define YYDEBUG(s, c)
#endif
#define STATE(name) yyc##name
/*** E re2c interface code ***/

/*** S emulate flex constructs */
#define BEGIN(state)    YYSETCONDITION(STATE(state))
#define YYSTATE         YYGETCONDITION()
#define yytext      ((char*)scanner_globals.yy_text)
#define yyleng      scanner_globals.yy_leng
/*** E emulate flex constructs */

#define CO_IS_OCT(c)    ((c)>='0' && (c)<='7')
#define CO_IS_HEX(c)    (((c)>='0' && (c)<='9') || ((c)>='a' && (c)<='f') || ((c)>='A' && (c)<='F'))

static void
co_scan_singlequoted_string(struct cval *cval)
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
co_scan_doublequoted_string(struct cval *cval)
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
co_scanner_openfile(int fd)
{
    scanner_globals.yy_in = fd;
    scanner_globals.yy_start = NULL;
    dstring code = dstring_readfd(fd);
    scanner_globals.yy_cursor = code;
    scanner_globals.yy_limit = code + strlen(code);

    return 0;
}

int
co_scanner_lex(struct cnode *yylval)
{
    scanner_globals.yy_text = YYCURSOR;

/*!re2c
re2c:yyfill:check = 0;

NUM         = [0-9]+;
FNUM        = ([0-9]*"."[0-9]+);
HNUM        = "0x"[0-9a-fA-F]+;
NAME        = [a-zA-Z_][a-zA-Z0-9_]*;
TOKENS      = [-+()=>/<*^,;:{}%];
WHITESPACE  = [ \t\n]+;
ANY_CHAR    = [^];

/* compute yyleng before each rule */
<!*> := yyleng = YYCURSOR - scanner_globals.yy_text;

<SC_INIT>NUM {
    yylval->u.val.u.ival = strtol(yytext, NULL, 0); 
    yylval->u.val.type = CVAL_IS_INT;
    yylval->type = IS_CONST;
    return T_NUM;
}

<SC_INIT>FNUM {
    yylval->u.val.u.fval = strtof(yytext, NULL);
    yylval->u.val.type = CVAL_IS_FLOAT;
    yylval->type = IS_CONST;
    return T_FNUM;
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

<SC_INIT>"try" {
    return T_TRY;
}

<SC_INIT>"catch" {
    return T_CATCH;
}

<SC_INIT>"throw" {
    return T_THROW;
}

<SC_INIT>"finally" {
    return T_FINALLY;
}

<SC_INIT>"None" {
    yylval->u.val.u.ival = 1;
    yylval->u.val.type = CVAL_IS_NONE;
    yylval->type = IS_CONST;
    return T_NONE;
}

<SC_INIT>"True" {
    yylval->u.val.u.ival = 1;
    yylval->u.val.type = CVAL_IS_BOOL;
    yylval->type = IS_CONST;
    return T_BOOL;
}

<SC_INIT>"False" {
    yylval->u.val.u.ival = 0;
    yylval->u.val.type = CVAL_IS_BOOL;
    yylval->type = IS_CONST;
    return T_BOOL;
}

<SC_INIT>["] {
    BEGIN(SC_DOUBLE_QUOTES);
    return T_IGNORED;
}

<SC_DOUBLE_QUOTES>["] {
    BEGIN(SC_INIT);
    return T_IGNORED;
}

<SC_DOUBLE_QUOTES>ANY_CHAR {
    if (YYCURSOR > YYLIMIT) {
        return 0;
    }
    if (yytext[0] == '\\' && YYCURSOR < YYLIMIT) {
        YYCURSOR++;
    }
    while (YYCURSOR < YYLIMIT) {
        switch (*YYCURSOR++) {
            case '"':
                break;
            case '\\':
                if (YYCURSOR < YYLIMIT) {
                    YYCURSOR++;
                }
                /* fall through */
            default:
                continue;
        }

        YYCURSOR--;
        break;
    }
    yyleng = YYCURSOR - scanner_globals.yy_text;
    yylval->u.val.u.str.val = xstrndup(yytext, yyleng);
    yylval->u.val.u.str.len = yyleng;
    co_scan_doublequoted_string(&yylval->u.val);
    yylval->u.val.type = CVAL_IS_STRING;
    yylval->type = IS_CONST;
    return T_STRING;
}

<SC_INIT>['] {
    while (1) {
        if (YYCURSOR > YYLIMIT) {
            return 0;
        }
        if (*YYCURSOR == '\'') {
            YYCURSOR++;
            yyleng = YYCURSOR - scanner_globals.yy_text;
            break;
        } else if (*YYCURSOR++ == '\\') {
            YYCURSOR++;
        }
    } 
    yylval->u.val.u.str.val = xstrndup(yytext + 1, yyleng - 2);
    yylval->u.val.u.str.len = yyleng - 2;
    co_scan_singlequoted_string(&yylval->u.val);
    yylval->u.val.type = CVAL_IS_STRING;
    yylval->type = IS_CONST;
    return T_STRING;
}

<SC_INIT>NAME {
    yylval->u.val.u.str.val = xstrndup(yytext, yyleng);
    yylval->u.val.u.str.len = yyleng;
    yylval->u.val.type = CVAL_IS_STRING;
    yylval->type = IS_VAR;
    return T_NAME;
}

<SC_INIT>TOKENS {
    return yytext[0];
}

<SC_INIT>"#".*   {
    return T_COMMENT;
}

<SC_INIT>WHITESPACE   { 
    return T_WHITESPACE;
}

<SC_INIT>ANY_CHAR   {
    coerror("invalid token: %s", yytext);
}
*/
}
