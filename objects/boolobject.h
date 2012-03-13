#ifndef OBJECTS_BOOLOBJECT_H
#define OBJECTS_BOOLOBJECT_H

#include "../co.h"
#include "../object.h"

struct COIntObject _CO_False, _CO_True; // Don't use these directly, using following two instead!
#define CO_False ((struct COObject *)&_CO_False)
#define CO_True ((struct COObject *)&_CO_True)

struct COTypeObject COBool_Type;
struct COObject *COBool_FromLong(long ok);

#endif
