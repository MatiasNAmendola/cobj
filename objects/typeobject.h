#ifndef OBJECTS_TYPEOBJECT_H
#define OBJECTS_TYPEOBJECT_H

#include "../object.h"

/* Object Methods */
typedef void (*deallocfunc) (COObject *);
typedef COObject *(*reprfunc)(COObject *);
typedef COObject *(*getattrfunc)(COObject *, char *);
typedef int (*setattrfunc) (COObject *, char *, COObject *);
typedef long (*hashfunc) (COObject *);
typedef COObject *(*comparefunc)(COObject *, COObject *, int);
typedef int (*visitfunc)(COObject *, void *);
typedef int (*traversefunc)(COObject *, visitfunc, void *);
typedef int (*inquiryfunc)(COObject *);
typedef COObject *(*unaryfunc)(COObject *);
typedef COObject *(*binaryfunc)(COObject *, COObject *);
typedef ssize_t(*lenfunc) (COObject *);
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

typedef struct {
    lenfunc sq_length;
    binaryfunc sq_concat;
} COSequenceInterface;

typedef struct {
    lenfunc mp_length;
    binaryfunc mp_subscript;
} COMappingInterface;
/* ! Object Interfaces */

struct _COTypeObject {
    COObject_HEAD;
    char *tp_name;              /* For printing */
    int tp_basicsize;           /* Following two are for allocation */
    int tp_itemsize;
    //int tp_flags;

    /* Basic Methods  */
    deallocfunc tp_dealloc;
    reprfunc tp_repr;
    getattrfunc tp_getattr;
    setattrfunc tp_setattr;
    hashfunc tp_hash;
    comparefunc tp_compare;
    traversefunc tp_traverse;
    inquiryfunc tp_clear;

    /* Interfaces for standard types */
    COIntInterface *tp_int_interface;
    COMappingInterface *tp_mapping_interface;
};

COTypeObject COType_Type;

#define COType_Check(co) (CO_TYPE(co) == &COType_Type)

void default_dealloc(COObject *this);

#endif
