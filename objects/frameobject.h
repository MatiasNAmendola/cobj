#ifndef OBJECTS_FRAMEOBJECT_H
#define OBJECTS_FRAMEOBJECT_H

#include "../object.h"

struct vm_stack;

typedef struct _COFrameObject {
    COObject_HEAD;
    struct vm_stack *vm_stack;
} COFrameObject;

COTypeObject COFrame_Type;

#define COFrame_Check(co) (CO_TYPE(co) == &COFrame_Type)

COObject *COFrame_New(void);
COObject *COFrame_Alloc(COObject *this, size_t size);
void COFrame_Free(COObject *this, COObject *co);
COObject *COFrame_Pop(COObject *this);
void COFrame_Push(COObject *this, COObject *co);
void COFrame_Destory(COObject *this);

#endif
