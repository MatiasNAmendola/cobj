#ifndef OBJECTS_FRAMEOBJECT_H
#define OBJECTS_FRAMEOBJECT_H
/*
 * Function execution frame.
 */

#include "../object.h"

#define FRAME_MAXBLOCKS 20

typedef struct {
    int fb_type;                /* kind of this block */
    int fb_handler;             /* where to jump to find handler */
    int fb_level;               /* stack level to pop to */
} COFrameBlock;

typedef struct _COFrameObject {
    COVarObject_HEAD;

    COObject *f_code;
    int f_lasti;                /* Last run instruction offset, defaults to -1. */
    COObject *f_prev;           /* previous frame, NULL indicates first frame */
    COObject *f_func;           /* function called on this frame */

    /*COObject *f_builtins;       *//* dict object for builtins names */
    COObject *f_locals;         /* dict object for names */

    int f_iblock;               /* index of block */
    COFrameBlock f_blockstack[FRAME_MAXBLOCKS];

    COObject **f_stacktop;      /* stack top, points to next free slot of stack */
    COObject *f_stack[1];       /* stack */
} COFrameObject;

COTypeObject COFrame_Type;

#define COFrame_Check(co) (CO_TYPE(co) == &COFrame_Type)

COObject *COFrame_New(COObject *prev, COObject *func);
COObject *COFrame_Pop(COObject *this);
void COFrame_Push(COObject *this, COObject *o);
void COFrame_Destory(COObject *this);
void COFrameBlock_Setup(COFrameObject *f, int type, int handler, int level);
COFrameBlock *COFrameBlock_Pop(COFrameObject *f);

#endif
