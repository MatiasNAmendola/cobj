#ifndef OBJECTS_INTOBJECT_H
#define OBJECTS_INTOBJECT_H

#include "../object.h"

typedef struct _COIntObject {
    COObject_HEAD;
    int long co_ival;
} COIntObject;

COTypeObject COInt_Type;

#define COInt_Check(co) (CO_TYPE(co) == &COInt_Type)

COObject *COInt_FromString(char *s, int base);
COObject *COInt_FromLong(long ival);
long COInt_AsLong(COObject *co);

#endif
