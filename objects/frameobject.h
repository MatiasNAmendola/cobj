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

    COObject *f_prev;           /* previous frame, NULL indicates first frame */
    COObject *f_code;
    int f_lasti;                /* Last run instruction offset, defaults to -1. */
    COObject *f_func;           /* function called on this frame */

    COObject *f_builtins;       /* dict of builtin symbol table */
    COObject *f_locals;         /* dict of local symbol table */

    int f_iblock;               /* index of block */
    COFrameBlock f_blockstack[FRAME_MAXBLOCKS];

    COObject **f_stacktop;      /* stack top, points to next free slot of stack */
    COObject **f_stack;         /* points after the last local */
    COObject *f_extraplus[1];   /* extra space (locals + stack), dynamically sized */
} COFrameObject;

COTypeObject COFrame_Type;

#define COFrame_Check(co) (CO_TYPE(co) == &COFrame_Type)

COObject *COFrame_New(COObject *prev, COObject *func, COObject *locals);
void COFrameBlock_Setup(COFrameObject *f, int type, int handler, int level);
COFrameBlock *COFrameBlock_Pop(COFrameObject *f);
int COFrame_Init(void);
void COFrame_Fini(void);

#endif
