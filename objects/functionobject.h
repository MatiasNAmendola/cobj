#ifndef OBJECTS_FUNCTIONOBJECT_H
#define OBJECTS_FUNCTIONOBJECT_H

#include "../object.h"
#include "../hash.h"
#include "../compile.h"

typedef struct _COFunctionObject {
    COObject_HEAD;
    COObject *func_name;
    HashTable upvalues;
    COObject *co_code;      /* string of bytecodes */
    struct co_opline_array *opline_array;
} COFunctionObject;

COTypeObject COFunction_Type;
COObject *COFunctionObject_New(COObject *func_name);

#endif
