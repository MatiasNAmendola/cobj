#include "co.h"

/* For debugging convenience. */
COObject *
COObject_repr(COObject *o)
{
    return CO_TYPE(o)->tp_repr(o);
}

void
COObject_dump(COObject *co)
{
    if (co == NULL) {
        fprintf(stderr, "NULL\n");
    } else {
        fprintf(stderr, "object:\n");
        fprintf(stderr, "    type: %s\n",
                CO_TYPE(co) == NULL ? "NULL" : CO_TYPE(co)->tp_name);
         COStrObject *s = COObject_repr(co);
        fprintf(stderr, "    repr: %s\n", s->co_sval);
        fprintf(stderr, "    refcnt: %d\n", co->co_refcnt);
    }
}

static COObject *
none_repr(COObject *co)
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
COObject_hash(COObject *co)
{
    COTypeObject *tp = co->co_type;
    if (tp->tp_hash != NULL) {
        return tp->tp_hash(co);
    }
    error("fatal");
    return -1;
}

COObject *
_COObject_New(COTypeObject *tp)
{
    COObject *co;
    co = (COObject *)COMem_MALLOC(tp->tp_basicsize);
    if (co == NULL) {
        // TODO errors
        return NULL;
    }

    return COObject_Init(co, tp);
}

COVarObject *
_COVarObject_New(COTypeObject *tp, size_t n)
{
    COVarObject *co;
    const size_t size = COObject_VAR_SIZE(tp, n);
    co = (COVarObject *)COMem_MALLOC(size);
    if (co == NULL) {
        // TODO errors
        return NULL;
    }
    return COVarObject_Init(co, tp, n);
}

void
COObject_print(COObject *co)
{
    COStrObject *s = (COStrObject *)CO_TYPE(co)->tp_repr(co);
    printf("%s\n", s->co_sval);
}
