#ifndef OBJECTS_THREADSTATE_H
#define OBJECTS_THREADSTATE_H
/**
 * Per thread state.
 */

#include "../object.h"
#include "../state.h"

typedef struct _COThreadStateObject {
    COObject_HEAD;
    COObject *modules;          /* dict of name => module */
    COObject *module_search_path;
} COThreadStateObject;

COTypeObject COThreadState_Type;

#define COThreadState_Check(co) (CO_TYPE(co) == &COThreadState_Type)

COThreadStateObject *COThreadState_New(void);

#endif
