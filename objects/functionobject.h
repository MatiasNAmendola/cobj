#ifndef OBJECTS_FUNCTIONOBJECT_H
#define OBJECTS_FUNCTIONOBJECT_H

#include "../object.h"
#include "../compile.h"

typedef struct _COFunctionObject {
    COObject_HEAD;
    COObject *func_name;
    COObject *func_upvalues;        /* dict of upvalues */
    struct co_opline_array *opline_array;
    COObject *func_code;            /* code object */
} COFunctionObject;

COTypeObject COFunction_Type;
COObject *COFunctionObject_New(COObject *func_name);

#endif
