#include "co.h"

typedef struct {
    FILE *fp;
    /* If fp == NULL, using following */
    COObject *str;
    char *ptr;
    char *end;
} WFILE;

typedef WFILE RFILE;

/* null like EOF in file, it's important */
#define TYPE_NULL       '0'
#define TYPE_NONE       'N'
#define TYPE_FALSE      'F'
#define TYPE_TRUE       'T'
#define TYPE_INT        'i'
#define TYPE_FLOAT      'f'
#define TYPE_STRING     's'
#define TYPE_TUPLE      '('
#define TYPE_LIST       '['
#define TYPE_DICT       '{'
#define TYPE_CODE       'c'
#define TYPE_OPLINE     'o'
#define TYPE_UNKNOW     '?'

#define w_byte(c, p)                        \
    if (((p)->fp)) putc((c), (p)->fp);      \
    else if ((p)->ptr != (p)->end)           \
        *(p)->ptr++ = (c);                  \
    else w_more(c, p)

static void w_cnode(struct cnode *node, WFILE *p);
static COObject *r_object(RFILE *p);

static void
w_more(int c, WFILE *p)
{
    if (p->str == NULL)
        return;

    size_t size, newsize;

    size = COBytes_Size(p->str);
    newsize = size + size + 1024;
    if (COBytes_Resize(p->str, newsize) != 0) {
        p->ptr = p->end = NULL;
    } else {
        p->ptr = COBytes_AsString(p->str) + size;
        p->end = COBytes_AsString(p->str) + newsize;
        *p->ptr++ = c;
    }
}

static void
w_int32(long x, WFILE *p)
{
    w_byte((char)(x & 0xff), p);
    w_byte((char)((x >> 8) & 0xff), p);
    w_byte((char)((x >> 16) & 0xff), p);
    w_byte((char)((x >> 24) & 0xff), p);
}

static void
w_int64(long x, WFILE *p)
{
    w_int32(x, p);
    w_int32(x >> 32, p);
}

static void
w_string(char *s, int n, WFILE *p)
{
    while (--n >= 0) {
        w_byte(*s, p);
        s++;
    }
}

static void
w_object(COObject *co, WFILE *p)
{
    if (co == NULL) {
        w_byte(TYPE_NULL, p);
    } else if (co == CO_None) {
        w_byte(TYPE_NONE, p);
    } else if (co == CO_False) {
        w_byte(TYPE_FALSE, p);
    } else if (co == CO_True) {
        w_byte(TYPE_TRUE, p);
    } else if (COInt_Check(co)) {
        w_byte(TYPE_INT, p);
        long x = COInt_AsLong(co);
        w_int64(x, p);
    } else if (COList_Check(co)) {
        w_byte(TYPE_LIST, p);
        size_t n = COList_Size(co);
        w_int64((long)n, p);
        for (int i = 0; i < n; i++) {
            w_object(COList_GetItem(co, i), p);
        }
    } else if (COTuple_Check(co)) {
        w_byte(TYPE_TUPLE, p);
        size_t n = COTuple_Size(co);
        w_int64((long)n, p);
        for (int i = 0; i < n; i++) {
            w_object(COTuple_GetItem(co, i), p);
        }
    } else if (COCode_Check(co)) {
        COCodeObject *code = (COCodeObject *)co;
        w_byte(TYPE_CODE, p);
        w_int64((long)code->co_numoftmpvars, p);
        w_object(code->co_oplines, p);
    } else if (COOpline_Check(co)) {
        COOplineObject *opline = (COOplineObject *)co;
        w_byte(TYPE_OPLINE, p);
        w_byte(opline->opcode, p);
        w_cnode(&opline->result, p);
        w_cnode(&opline->op1, p);
        w_cnode(&opline->op2, p);
    } else if (COStr_Check(co)) {
        w_byte(TYPE_STRING, p);
        size_t n = CO_SIZE(co);
        w_int64((long)n, p);
        w_string(COStr_AsString(co), (int)n, p);
    } else {
        /*COObject_dump(co);*/
        /*error("unknow object to write");*/
        w_byte(TYPE_UNKNOW, p);
    }
}

static void
w_cnode(struct cnode *node, WFILE *p)
{
    w_byte(node->type, p);
    if (node->type == IS_CONST || node->type == IS_VAR) {
        w_object(node->u.co, p);
    } else if (node->type == IS_TMP_VAR) {
        w_int64(node->u.var, p);
    } else {
        w_int64(node->u.opline_num, p);
    }
}

static int
r_string(char *s, int n, RFILE *p)
{
    int read, left;
    if (p->fp != NULL) {
        read = (int)fread(s, 1, n, p->fp);
    } else {
        left = (int)(p->end - p->ptr);
        read = (left < n) ? left : n;
        memcpy(s, p->ptr, read);
        p->ptr += read;
    }
    return read;
}

static int
r_byte(RFILE *p)
{
    int c = EOF;
    char ch;
    int n = r_string((char *)&ch, 1, p);
    if (n > 0)
        c = ch;
    return c;
}

static long
r_int32(RFILE *p)
{
    register long x;
    unsigned char buffer[4];
    r_string((char *)buffer, 4, p);
    x = buffer[0];
    x |= (long)buffer[1] << 8;
    x |= (long)buffer[2] << 16;
    x |= (long)buffer[3] << 24;

    return x;
}

static long
r_int64(RFILE *p)
{
    long lo4 = r_int32(p);
    long hi4 = r_int32(p);
    long x = (hi4 << 32) | (lo4 & 0xFFFFFFFFL);
    return x;
}

static void
r_cnode(struct cnode *node, RFILE *p)
{
    node->type = r_byte(p);
    if (node->type == IS_CONST || node->type == IS_VAR) {
        node->u.co = r_object(p);
    } else if (node->type == IS_TMP_VAR) {
        node->u.var = r_int64(p);
    } else {
        node->u.opline_num = r_int64(p);
    }
}

static COObject *
r_object(RFILE *p)
{
    int type;
    COObject *rs;
    type = r_byte(p);

    switch (type) {
    case EOF:
        rs = NULL;
        break;
    case TYPE_NULL:
        rs = NULL;
        break;
    case TYPE_NONE:
        rs = CO_None;
        break;
    case TYPE_FALSE:
        rs = CO_False;
        break;
    case TYPE_TRUE:
        rs = CO_True;
        break;
    case TYPE_INT:
        {
            long n = r_int64(p);
            return COInt_FromLong(n);
        }
        break;
    case TYPE_LIST:
        {
            size_t n = r_int64(p);
            rs = COList_New(n);
            for (int i = 0; i < n; i++) {
                COObject *v = r_object(p);
                if (v != NULL) {
                    COList_SetItem(rs, i, v);
                }
            }
        }
        break;
    case TYPE_TUPLE:
        {
            size_t n = r_int64(p);
            rs = COTuple_New(n);
            for (int i = 0; i < n; i++) {
                COObject *v = r_object(p);
                if (v != NULL) {
                    COTuple_SetItem(rs, i, v);
                }
            }
        }
        break;
    case TYPE_CODE:
        {
            int numoftmpvars;
            COObject *oplines = NULL;
            numoftmpvars = r_int64(p);
            oplines = r_object(p);
            rs = COCode_New(oplines, numoftmpvars);
        }
        break;
    case TYPE_OPLINE:
        {
            COOplineObject *opline = (COOplineObject *)COOpline_New();
            opline->opcode = r_byte(p);
            r_cnode(&opline->result, p);
            r_cnode(&opline->op1, p);
            r_cnode(&opline->op2, p);
            rs = (COObject *)opline;
        }
        break;
    case TYPE_STRING:
        {
            int n = r_int64(p);
            rs = COStr_FromStringN(NULL, n);
            if (r_string(COStr_AsString(rs), (int)n, p) != n) {
                error("EOF read where string object expected");
                rs = NULL;
            }
        }
        break;
    default:
        /*error("unknow object to read: %d", type);*/
        return NULL;
    }
    return rs;
}

COObject *
COObject_serialize(COObject *co)
{
    WFILE p;
    p.fp = NULL;
    p.str = COBytes_FromStringN(NULL, 0);
    p.ptr = p.end = NULL;
    w_object(co, &p);
    COBytes_Resize(p.str, p.ptr - COBytes_AsString(p.str));
    return p.str;
}

COObject *
COObject_unserialize(COObject *s)
{
    RFILE p;
    p.fp = NULL;
    p.str = s;
    p.ptr = COBytes_AsString(p.str);
    p.end = p.ptr + COBytes_Size(p.str);

    return r_object(&p);
}

COObject *
COObject_serializeToFile(COObject *co, FILE *fp)
{
    WFILE p;
    p.fp = fp;
    w_object(co, &p);
    return p.str;
}

COObject *
COObject_unserializeFromFile(FILE *fp)
{
    RFILE p;
    p.fp = fp;
    return r_object(&p);
}
