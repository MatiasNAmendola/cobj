#include "object.h"
#include "objects/strobject.h"

/* For debugging convenience. */
void
coobject_print(struct COObject *co)
{
    if (co == NULL) {
        fprintf(stderr, "NULL\n");
    } else {
        fprintf(stderr, "object:\n");
        fprintf(stderr, "    type: %s\n", CO_TYPE(co) == NULL ? "NULL" : CO_TYPE(co)->tp_name);
        struct COStrObject *s = (struct COStrObject *)CO_TYPE(co)->tp_repr(co);
        fprintf(stderr, "    repr: %s\n", s->co_str);
    }
}

static struct COObject *
none_repr(struct COObject *co)
{
    return COStrObject_FromString("None");
}

static struct COTypeObject CONone_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "NoneType",
    0,
    0,
    none_repr,                      /* tp_repr */
    0,                              /* tp_getattr */
    0,                              /* tp_setattr */
};

struct COObject _CO_None = COObject_HEAD_INIT(&CONone_Type);
