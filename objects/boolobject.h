#ifndef OBJECTS_BOOLOBJECT_H
#define OBJECTS_BOOLOBJECT_H

#include "../object.h"

COIntObject _CO_False, _CO_True; // Don't use these directly, using following two instead!
#define CO_False ((COObject *)&_CO_False)
#define CO_True ((COObject *)&_CO_True)

struct COTypeObject COBool_Type;
COObject *COBool_FromLong(long ok);

#endif
