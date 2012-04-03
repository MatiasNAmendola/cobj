#include "co.h"

COObject *
COObject_repr(COObject *o)
{
    return CO_TYPE(o)->tp_repr(o);
}

void
COObject_dump(COObject *o)
{
    if (o == NULL) {
        fprintf(stderr, "NULL\n");
    } else {
        fprintf(stderr, "object:\n");
        fprintf(stderr, "    type: %s\n",
                CO_TYPE(o) == NULL ? "NULL" : CO_TYPE(o)->tp_name);
        COStrObject *s = (COStrObject *)COObject_repr(o);
        fprintf(stderr, "    repr: %s\n", s->co_sval);
        fprintf(stderr, "    refcnt: %d\n", o->co_refcnt);
    }
}

static COObject *
none_repr(COObject *o)
{
    return COStr_FromString("None");
}

static COTypeObject CONone_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "NoneType",
    0,
    0,
    none_repr,                  /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
};

COObject _CO_None = COObject_HEAD_INIT(&CONone_Type);

long
_CO_HashPointer(void *p)
{
    long x;
    size_t y = (size_t)p;
    /* bottom 3 or 4 bits are likely to be 0; rotate y by 4 to avoid
       excessive hash collisions for dicts and sets */
    y = (y >> 4) | (y << (8 * sizeof(void *) - 4)); 
    x = (long)y;
    if (x == -1)
        x = -2;
    return x;
}

long
COObject_hash(COObject *o)
{
    COTypeObject *tp = o->co_type;
    if (tp->tp_hash != NULL) {
        return tp->tp_hash(o);
    }
    // TODO this is only tmp fix
    return _CO_HashPointer(o);
}

COObject *
_COObject_New(COTypeObject *tp)
{
    COObject *o;
    o = (COObject *)COMem_MALLOC(tp->tp_basicsize);
    if (o == NULL) {
        // TODO errors
        return NULL;
    }

    return COObject_Init(o, tp);
}

COVarObject *
_COVarObject_New(COTypeObject *tp, size_t n)
{
    COVarObject *o;
    const size_t size = COObject_VAR_SIZE(tp, n);
    o = (COVarObject *)COMem_MALLOC(size);
    if (o == NULL) {
        // TODO errors
        return NULL;
    }
    return COVarObject_Init(o, tp, n);
}

void
COObject_print(COObject *o)
{
    COStrObject *s = (COStrObject *)CO_TYPE(o)->tp_repr(o);
    printf("%s\n", s->co_sval);
}
