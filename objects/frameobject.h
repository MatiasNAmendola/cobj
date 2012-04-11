#ifndef OBJECTS_FRAMEOBJECT_H
#define OBJECTS_FRAMEOBJECT_H
/*
 * Function execution frame.
 */

#include "../object.h"

typedef struct _COFrameObject {
    COVarObject_HEAD;
    unsigned char *firstcode;
    unsigned char *bytecode;
    COObject *f_prev;           /* previous frame, NULL indicates first frame */
    COObject *f_func;           /* function called on this frame */
    COObject *f_locals;         /* dict object for names */
    COObject **f_stacktop;      /* stack top, points to next free slot of stack */
    COObject *f_stack[1];       /* stack */
} COFrameObject;

COTypeObject COFrame_Type;

#define COFrame_Check(co) (CO_TYPE(co) == &COFrame_Type)

COObject *COFrame_New(void);
COObject *COFrame_Pop(COObject *this);
void COFrame_Push(COObject *this, COObject *o);
void COFrame_Destory(COObject *this);

#endif
