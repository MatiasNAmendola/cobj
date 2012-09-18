#include "cobj.h"

COGlobalState *COGlobalState_Current;

COGlobalState *
COGlobalState_New()
{
    COGlobalState *gstate = (COGlobalState *)malloc(sizeof(COGlobalState));
    if (!gstate) {
        return NULL;
    }

    gstate->curexc_type = NULL;
    gstate->curexc_value = NULL;
    gstate->curexc_traceback = NULL;
    return gstate;
}

void
COGlobalState_Delete(COGlobalState *gstate)
{
    if (!gstate)
        return;

    CO_XDECREF(gstate->curexc_type);
    CO_XDECREF(gstate->curexc_value);
    CO_XDECREF(gstate->curexc_traceback);
}
