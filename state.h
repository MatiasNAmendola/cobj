#ifndef STATE_H
#define STATE_H
/**
 * State etc.
 */

#include "object.h"
#include "stack.h"

typedef struct _COThreadState {
    COObject *next_func;
    struct co_exec_data *current_exec_data;
    co_stack argument_stack;

    COObject *frame;

    COObject *curexc_type;
    COObject *curexc_value;
    COObject *curexc_traceback;
} COThreadState;

COThreadState *threadstate_current;
#define TS(o)    (threadstate_current)->o

COThreadState *COThreadState_New();

#endif
