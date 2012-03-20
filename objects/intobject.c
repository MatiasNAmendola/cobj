#include "../co.h"

static COObject *
int_repr(COIntObject *this)
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
    return COStr_FromStringN(p, bufend - p);
}

COTypeObject COInt_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "int",
    sizeof(COIntObject),
    0,
    (reprfunc)int_repr,         /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
};

long
COInt_AsLong(COObject *co)
{
    return ((COIntObject *)co)->co_ival;
}

COObject *
COInt_FromString(char *s, int base)
{
    COIntObject *num;

    if (base != 0 && (base < 2 || base > 36)) {
        // TODO errors
        return NULL;
    }

    num = xmalloc(sizeof(COIntObject));
    COObject_Init(num, &COInt_Type);
    num->co_ival = strtol(s, NULL, base);
    return (COObject *)num;
}

COObject *
COInt_FromLong(long ival)
{
    COIntObject *num;

    num = xmalloc(sizeof(COIntObject));
    COObject_Init(num, &COInt_Type);
    num->co_ival = ival;
    return (COObject *)num;
}
