#include "intobject.h"

static struct COObject *
int_repr(struct COIntObject *this)
{
    char buf[sizeof(long) * 8 / 3 + 6], *p, *bufend;
    long n = this->co_ival;
    unsigned long absn;
    p = bufend = buf + sizeof(buf);
    absn = n < 0 ? 0UL - n : n;
    do {
        *--p = '0' + (char)(absn % 10);
        absn /= 10;
    } while (absn);
    if (n < 0)
        *--p = '-';
    return COStr_FromStingN(p, bufend - p);
}

struct COTypeObject COInt_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "int",
    sizeof(struct COIntObject),
    0,
    (reprfunc)int_repr,                      /* tp_repr */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
};

long
COInt_AsLong(struct COObject *co)
{
    return ((struct COIntObject *)co)->co_ival;
}

struct COObject *
COInt_FromString(char *s, int base)
{
    struct COIntObject *num;

    if (base != 0 && (base < 2 || base > 36)) {
        // TODO errors
        return NULL;
    }

    num = xmalloc(sizeof(struct COIntObject));
    CO_INIT(num, &COInt_Type);
    num->co_ival = strtol(s, NULL, base);
    return (struct COObject *)num;
}


struct COObject *
COInt_FromLong(long ival)
{
    struct COIntObject *num;

    num = xmalloc(sizeof(struct COIntObject));
    CO_INIT(num, &COInt_Type);
    num->co_ival = ival;
    return (struct COObject *)num;
}
