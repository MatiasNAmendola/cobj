#ifndef STATE_H
#define STATE_H
/**
 * State etc.
 */

#include "object.h"
#include "objects/frameobject.h"

typedef struct _COState {
    COObject *curexc_type;
    COObject *curexc_value;
    COObject *curexc_traceback;
} COState;

COState *state_current;
#define TS(o)    (state_current)->o

COState *COState_New();
void COState_Delete(COState *state);
void COState_DeleteCurrent();

#endif
