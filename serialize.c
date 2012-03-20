#include "co.h"

typedef struct {
    COObject *str;
} WFILE;

typedef WFILE RFILE;

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
#define TYPE_UNKNOW     '?'
#define TYPE_OPLINE     'o'

#define w_byte(c, p) if

static void
w_more(int c, WFILE *p)
{

}

static void
w_object(COObject *co, WFILE *p)
{
    if (co == NULL) {
    }
}

static COObject *
r_object(RFILE *p)
{
    COObject *co;
    return co;
}

COObject *
COObject_serialize(COObject *co)
{
    WFILE p;
    w_object(co, &p);
    return p.str;
}

COObject *
COObject_unserialize(COObject *s)
{
    RFILE p;
    p.str = s;

    COObject *co;

    co = r_object(&p);
    return co;
}
