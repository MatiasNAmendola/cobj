#ifndef STATE_H
#define STATE_H
/**
 * State etc.
 */

#include "object.h"
#include "stack.h"

typedef struct _COState {
    COObject *next_func;
    struct co_exec_data *current_exec_data;
    co_stack argument_stack;

    COObject *curexc_type;
    COObject *curexc_value;
    COObject *curexc_traceback;
} COState;

COState *state_current;
#define S(o)    (state_current)->o

COState *COState_New();

#endif
