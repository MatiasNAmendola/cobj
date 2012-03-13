#ifndef OBJECTS_FUNCTIONOBJECT_H
#define OBJECTS_FUNCTIONOBJECT_H

#include "../co.h"
#include "../object.h"
#include "../hash.h"
#include "../compile.h"

struct COFunctionObject {
    COObject_HEAD;
    struct COObject *func_name;
    HashTable upvalues;
    struct co_opline_array *opline_array;
};

struct COTypeObject COFunction_Type;
struct COObject *COFunctionObject_New(struct COObject *func_name);

#endif
