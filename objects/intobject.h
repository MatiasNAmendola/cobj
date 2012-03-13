#ifndef OBJECTS_INTOBJECT_H
#define OBJECTS_INTOBJECT_H

#include "../co.h"
#include "../object.h"

struct COIntObject {
    COObject_HEAD;
    int long co_ival;
};

struct COTypeObject COInt_Type;
struct COObject *COInt_FromString(char *s, int base);
struct COObject *COInt_FromLong(long ival);
long COInt_AsLong(struct COObject *co);

#endif