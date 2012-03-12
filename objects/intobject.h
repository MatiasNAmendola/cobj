#ifndef OBJECTS_INTOBJECT_H
#define OBJECTS_INTOBJECT_H

#include "../co.h"
#include "../object.h"

struct COIntObject {
    COObject_HEAD;
    int long ob_int;
};

struct COTypeObject COInt_Type;

#endif
