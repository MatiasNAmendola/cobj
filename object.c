#include "co.h"

/* For debugging convenience. */
void
COObject_dump(COObject *co)
{
    if (co == NULL) {
        fprintf(stderr, "NULL\n");
    } else {
        fprintf(stderr, "object:\n");
        fprintf(stderr, "    type: %s\n",
                CO_TYPE(co) == NULL ? "NULL" : CO_TYPE(co)->tp_name);
        COStrObject *s = (COStrObject *)CO_TYPE(co)->tp_repr(co);
        fprintf(stderr, "    repr: %s\n", s->co_sval);
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
