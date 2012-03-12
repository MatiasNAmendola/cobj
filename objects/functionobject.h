#ifndef OBJECTS_FUNCTIONOBJECT_H
#define OBJECTS_FUNCTIONOBJECT_H

#include "../co.h"
#include "../object.h"

struct COFunctionObject {
    COObject_HEAD;
    struct COObject *function_name;
};

struct COTypeObject COFunction_Type;

#endif
