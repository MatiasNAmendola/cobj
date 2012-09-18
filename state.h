#ifndef STATE_H
#define STATE_H
/**
 * State etc.
 */

#include "object.h"
#include "objects/frameobject.h"
#include "objects/threadstateobject.h"

typedef struct _COGlobalState {
    COThreadStateObject *mainthread;
    COObject *curexc_type;
    COObject *curexc_value;
    COObject *curexc_traceback;
} COGlobalState;

COGlobalState *COGlobalState_Current;
#define GS(o) (COGlobalState_Current)->o

COGlobalState *COGlobalState_New();
void COGlobalState_Delete(COGlobalState *gstate);

#endif
