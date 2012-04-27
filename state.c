#include "co.h"

COThreadState *threadstate_current = NULL;

COThreadState *
COThreadState_New()
{
    COThreadState *state = (COThreadState *)malloc(sizeof(COThreadState));
    if (state != NULL) {
        // TODO
    }
    state->funcargs = COList_New(0);
    state->frame = NULL;

    state->curexc_type = NULL;
    state->curexc_value = NULL;
    state->curexc_traceback = NULL;

    return state;
}

void
COThreadState_Delete(COThreadState *state)
{
    if (!state)
        return;

    CO_XDECREF(state->mainfunc);
    CO_XDECREF(state->funcargs);
    CO_XDECREF(state->frame);

    CO_XDECREF(state->curexc_type);
    CO_XDECREF(state->curexc_value);
    CO_XDECREF(state->curexc_traceback);
}

void
COThreadState_DeleteCurrent()
{
    COThreadState_Delete(threadstate_current);
}
