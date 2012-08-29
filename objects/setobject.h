#ifndef OBJECTS_SETOBJECT_H
#define OBJECTS_SETOBJECT_H
/**
 * Set object.
 */

#include "../object.h"

struct setentry {
    /* Cached hash of the key. */
    long hash;
    COObject *key;
};

#define COSet_MINSIZE 8

typedef struct _COSetObject {
    COObject_HEAD;

    ssize_t fill;               /* #active + #dummy */
    ssize_t used;               /* #active */

    /* The table contains mask +1 slots, and that's a power of 2.
     * We store the mask instead of the size because the mask is more frequently
     * needed.
     */
    ssize_t mask;

    /* Points to smalltable for small tables, else to additional malloc'ed
     * memory, and is never NULL.
     */
    struct setentry *table;
    struct setentry *(*lookup) (struct _COSetObject * so, COObject *key,
                                long hash);
    struct setentry smalltable[COSet_MINSIZE];
} COSetObject;

COTypeObject COSet_Type;

#define COSet_Check(co) (CO_TYPE(co) == &COSet_Type)

#define COSet_GET_SIZE(co) (((COSetObject *)(co))->used)

COObject *COSet_New(COObject *iterable);
ssize_t COSet_Size(COObject *this);

#endif
