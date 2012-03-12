#ifndef OBJECTS_STROBJECT_H
#define OBJECTS_STROBJECT_H

#include "../co.h"
#include "../object.h"

struct COStrObject {
    COObject_HEAD;
    size_t co_len;
    char co_str[1];
};

struct COTypeObject COStr_Type;

struct COObject *COStrObject_FromString(const char *s);

#endif
