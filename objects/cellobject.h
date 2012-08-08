#ifndef OBJECTS_CELLOBJECT_H
#define OBJECTS_CELLOBJECT_H
/**
 * Cell object interface
 */

#include "../object.h"

typedef struct _COCellObject {
    COObject_HEAD;
    COObject *co_ref;
} COCellObject;

COTypeObject COCell_Type;

#define COCell_Check(co) (CO_TYPE(co) == &COCell_Type)

COObject *COCell_New(COObject *);
COObject *COCell_Get(COObject *);
int COCell_Set(COObject *, COObject *);

#define COCell_GET(o)       (((COCellObject *)o)->co_ref)
#define COCell_SET(o, v)    (((COCellObject *)o)->co_ref = v)

#endif
