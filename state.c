#include "co.h"

COThreadState *threadstate_current = NULL;

COThreadState *
COThreadState_New()
{
    COThreadState *state = (COThreadState *)malloc(sizeof(COThreadState));
    if (state != NULL) {
        // TODO
    }

    return state;
}
