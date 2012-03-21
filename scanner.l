#include "co.h"

struct co_scanner_globals {
    unsigned int yy_leng;
    char *yy_text;
    char *yy_cursor;
    char *yy_marker;
    char *yy_limit;
    int yy_state;
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
#ifdef YY_DEBUG
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
co_scan_singlequoted_string(COStrObject *co)
{
    register char *s, *t;
    char *end;

    s = t = co->co_sval;
    end = s + CO_SIZE(co);
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
                CO_SIZE(co)--;
                break;
            case '\\':
                *t++ = *s;
                CO_SIZE(co)--;
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
co_scan_doublequoted_string(COStrObject *co)
{
    register char *s, *t;
    char *end;

    s = t = co->co_sval;
    end = s + CO_SIZE(co);
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
                CO_SIZE(co)--;
                break;
            case 'b':
                *t++ = '\b';
                CO_SIZE(co)--;
                break;
            case 'f':
                *t++ = '\f';
                CO_SIZE(co)--;
                break;
            case 'n':
                *t++ = '\n';
                CO_SIZE(co)--;
                break;
            case 'r':
                *t++ = '\r';
                CO_SIZE(co)--;
                break;
            case 't':
                *t++ = '\t';
                CO_SIZE(co)--;
                break;
            case 'v':
                *t++ = '\v';
                CO_SIZE(co)--;
                break;
            case '"':
                *t++ = '"';
                CO_SIZE(co)--;
                break;
            case '\\':
                *t++ = *s;
                CO_SIZE(co)--;
                break;
            case 'x':
                if (CO_IS_HEX(*(s+1)) && CO_IS_HEX(*(s+2))) {
                    char hex_buf[3] = {0, 0, 0};
                    CO_SIZE(co)--; /* for the 'x' */

                    hex_buf[0] = *(++s);
                    CO_SIZE(co)--;
                    hex_buf[1] = *(++s);
                    CO_SIZE(co)--;
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
                    CO_SIZE(co)--;
                    if (CO_IS_OCT(*(s+1))) {
                        octal_buf[1] = *(++s);
                        CO_SIZE(co)--;
                        if (CO_IS_OCT(*(s+1))) {
                            octal_buf[2] = *(++s);
                            CO_SIZE(co)--;
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
    // TODO
}

void
co_scanner_shutdown()
{
    // TODO
}

int
co_scanner_setfile(COObject *f)
{
    return co_scanner_setcode(COBytes_AsString(COFile_Read(f, -1)));
}

int
co_scanner_setcode(char *code)
{
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
TOKENS      = [-+()=>/<*^,;:{}%\[\]];
WHITESPACE  = [ \t]+;
NEWLINES     = [\n];
ANY_CHAR    = [^];

/* compute yyleng before each rule */
<!*> := yyleng = YYCURSOR - scanner_globals.yy_text;

<SC_INIT>NUM {
    yylval->u.co = COInt_FromString(yytext, 0);
    yylval->type = IS_CONST;
    return T_NUM;
}

<SC_INIT>FNUM {
    yylval->u.co = COFloat_FromString(yytext);
    yylval->type = IS_CONST;
    return T_FNUM;
}

<SC_INIT>"if" {
    return T_IF;
}

<SC_INIT>"else" {
    return T_ELSE;
}

<SC_INIT>"elif" {
    return T_ELIF;
}

<SC_INIT>"while" {
    return T_WHILE;
}

<SC_INIT>"func" {
    return T_FUNC;
}

<SC_INIT>"end" {
    return T_END;
}

<SC_INIT>"print" {
    return T_PRINT;
}

<SC_INIT>"return" {
    return T_RETURN;
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
    yylval->u.co = CO_None;
    yylval->type = IS_CONST;
    return T_NONE;
}

<SC_INIT>"True" {
    yylval->u.co = CO_True;
    yylval->type = IS_CONST;
    return T_BOOL;
}

<SC_INIT>"False" {
    yylval->u.co = CO_False;
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
    yylval->u.co = COStr_FromStringN(yytext, yyleng);
    co_scan_doublequoted_string((COStrObject *)yylval->u.co);
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
    yylval->u.co = COStr_FromStringN(yytext + 1, yyleng -2);
    co_scan_singlequoted_string((COStrObject *)yylval->u.co);
    yylval->type = IS_CONST;
    return T_STRING;
}

<SC_INIT>NAME {
    yylval->u.co = COStr_FromStringN(yytext, yyleng);
    yylval->type = IS_VAR;
    return T_NAME;
}

<SC_INIT>"<<" {
    return T_SL;
}

<SC_INIT>">>" {
    return T_SR;
}

<SC_INIT>"+=" {
    return T_ADD_ASSIGN;
}

<SC_INIT>"-=" {
    return T_SUB_ASSIGN;
}

<SC_INIT>"*=" {
    return T_MUL_ASSIGN;
}

<SC_INIT>"/=" {
    return T_DIV_ASSIGN;
}

<SC_INIT>"%=" {
    return T_MOD_ASSIGN;
}

<SC_INIT>"<<=" {
    return T_SL_ASSIGN;
}

<SC_INIT>">>=" {
    return T_SR_ASSIGN;
}

<SC_INIT>"==" {
    return T_EQUAL;
}

<SC_INIT>"!=" {
    return T_NOT_EQUAL;
}

<SC_INIT>"<=" {
    return T_SMALLER_OR_EQUAL;
}

<SC_INIT>">=" {
    return T_GREATER_OR_EQUAL;
}

<SC_INIT>"[" {
    return yytext[0];
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

<SC_INIT>NEWLINES {
    return T_NEWLINES;
}

<SC_INIT>ANY_CHAR   {
    coerror("invalid token: %s", yytext);
}
*/
}
