#ifndef OBJECTS_STACKOBJECT_H
#define OBJECTS_STACKOBJECT_H

#include "../object.h"

struct vm_stack;

typedef struct _COStackObject {
    COObject_HEAD;
    struct vm_stack *vm_stack;
} COStackObject;

COTypeObject COStack_Type;

#define COStack_Check(co) (CO_TYPE(co) == &COStack_Type)

COObject *COStack_New(void);
COObject *COStack_Alloc(COObject *this, size_t size);
void COStack_Free(COObject *this, COObject *co);
COObject *COStack_Pop(COObject *this);
void COStack_Push(COObject *this, COObject *co);
void COStack_Destory(COObject *this);

#endif
