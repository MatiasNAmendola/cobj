#ifndef OBJECTS_OPLINEOBJECT_H
#define OBJECTS_OPLINEOBJECT_H
/**
 * Quadruple three-address instruction.
 */

#include "../object.h"
#include "../compile.h"

typedef struct _COOplineObject {
    COObject_HEAD;
    uchar opcode;
    struct cnode arg1;
    struct cnode arg2;
    struct cnode result;
} COOplineObject;

COTypeObject COOpline_Type;

#define COOpline_Check(co) (CO_TYPE(co) == &COOpline_Type)

COObject *COOpline_New();

#endif
