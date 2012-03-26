#include "co.h"

COState *state_current = NULL;

COState *
COState_New()
{
    COState *state = (COState *)malloc(sizeof(COState));
    if (state != NULL) {
        // TODO
    }

    return state;
}
