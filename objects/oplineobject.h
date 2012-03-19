#ifndef OBJECTS_OPLINEOBJECT_H
#define OBJECTS_OPLINEOBJECT_H

#include "../object.h"
#include "../compile.h"

typedef struct _COOplineObject {
    COObject_HEAD;
    uchar opcode;
    struct cnode result;
    struct cnode op1;
    struct cnode op2;
} COOplineObject;

COTypeObject COOpline_Type;

COObject *COOpline_New();

#endif
