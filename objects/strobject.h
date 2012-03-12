#ifndef OBJECTS_STROBJECT_H
#define OBJECTS_STROBJECT_H

#include "../co.h"
#include "../object.h"

struct COStrObject {
    COObject_HEAD;
    size_t len;
    char *str;
};

struct COTypeObject COStr_Type;

#endif
