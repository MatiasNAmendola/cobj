#ifndef OBJECTS_TYPEOBJECT_H
#define OBJECTS_TYPEOBJECT_H

#include "../object.h"

typedef COObject *(*reprfunc)(COObject *);
typedef COObject *(*getattrfunc)(COObject *, char *);
typedef int (*setattrfunc)(COObject *, char *, COObject *);

struct _COTypeObject {
    COObject_HEAD;
    char *tp_name;              /* For printing */
    int tp_basicsize;           /* Following two are for allocation */
    int tp_itemsize;

    /* Methods */
    reprfunc tp_repr;
    getattrfunc tp_getattr;
    setattrfunc tp_setattr;
};

COTypeObject COType_Type;

#endif
