#include "cobj.h"

COState *state_current = NULL;

COState *
COState_New()
{
    COState *state = (COState *)malloc(sizeof(COState));
    if (!state) {
        return NULL;
    }
    state->frame = NULL;

    state->curexc_type = NULL;
    state->curexc_value = NULL;
    state->curexc_traceback = NULL;

    return state;
}

void
COState_Delete(COState *state)
{
    if (!state)
        return;

    CO_XDECREF(state->frame);

    CO_XDECREF(state->curexc_type);
    CO_XDECREF(state->curexc_value);
    CO_XDECREF(state->curexc_traceback);
}

void
COState_DeleteCurrent()
{
    COState_Delete(state_current);
}
