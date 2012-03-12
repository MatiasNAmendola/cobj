#ifndef OBJECTS_TYPEOBJECT_H
#define OBJECTS_TYPEOBJECT_H

#include "../co.h"
#include "../object.h"

typedef struct COObject *(*reprfunc)(struct COObject *);
typedef struct COObject *(*getattrfunc)(struct COObject *, char *);
typedef int (*setattrfunc)(struct COObject *, char *, struct COObject *);

struct COTypeObject {
    COObject_HEAD;
    char *tp_name;              /* For printing */
    int tp_basicsize;           /* Following two are for allocation */
    int tp_itemsize;

    /* Methods */
    reprfunc tp_repr;
    getattrfunc tp_getattr;
    setattrfunc tp_setattr;
};

struct COTypeObject COType_Type;

#endif

