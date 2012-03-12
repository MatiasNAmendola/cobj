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
