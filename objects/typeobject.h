#ifndef OBJECTS_TYPEOBJECT_H
#define OBJECTS_TYPEOBJECT_H

#include "../object.h"

/* Object Methods */
typedef COObject *(*reprfunc)(COObject *);
typedef COObject *(*getattrfunc)(COObject *, char *);
typedef int (*setattrfunc) (COObject *, char *, COObject *);
typedef long (*hashfunc) (COObject *);
typedef COObject *(*richcmpfunc)(COObject *, COObject *, int);
typedef COObject *(*unaryfunc)(COObject *);
typedef COObject *(*binaryfunc)(COObject *, COObject *);
/* ! Object Methods */

/* Object Interfaces */
typedef struct {
    binaryfunc int_add;
    binaryfunc int_sub;
    binaryfunc int_mul;
    binaryfunc int_div;
    binaryfunc int_mod;
    binaryfunc int_lshift;
    binaryfunc int_rshift;
    unaryfunc int_neg;
    unaryfunc int_invert;
} COIntInterface;
/* ! Object Interfaces */

struct _COTypeObject {
    COObject_HEAD;
    char *tp_name;              /* For printing */
    int tp_basicsize;           /* Following two are for allocation */
    int tp_itemsize;

    /* Basic Methods  */
    reprfunc tp_repr;
    getattrfunc tp_getattr;
    setattrfunc tp_setattr;
    hashfunc tp_hash;
    richcmpfunc tp_richcompare;

    /* Interfaces for standard types */
    COIntInterface *tp_int_interface;
};

COTypeObject COType_Type;

#define COType_Check(co) (CO_TYPE(co) == &COType_Type)

#endif
