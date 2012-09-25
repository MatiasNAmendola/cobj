#ifndef OBJECTS_FLOATOBJECT_H
#define OBJECTS_FLOATOBJECT_H

#include "../object.h"

typedef struct _COFloatObject {
    COObject_HEAD;
    double co_fval;
} COFloatObject;

COTypeObject COFloat_Type;

#define COFloat_Check(co) (CO_TYPE(co) == &COFloat_Type)

#define COFloat_AS_DOUBLE(o)    (((COFloatObject *)(o))->co_fval)

COObject *COFloat_FromDouble(double);
COObject *COFloat_FromString(char *s);
int COFloat_ClearFreeList(void);
COObject *COFloat_Fini(void);

#endif
