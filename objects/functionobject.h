#ifndef OBJECTS_FUNCTIONOBJECT_H
#define OBJECTS_FUNCTIONOBJECT_H

#include "../object.h"
#include "../hash.h"
#include "../compile.h"

struct COFunctionObject {
    COObject_HEAD;
    COObject *func_name;
    HashTable upvalues;
    struct co_opline_array *opline_array;
};

struct COTypeObject COFunction_Type;
COObject *COFunctionObject_New(COObject *func_name);

#endif
