#ifndef OBJECT_H
#define OBJECT_H
/*
 * Object interface (Object, Type, Value)
 *
 * !!! This object system implementation is based on Python's objects code.
 *
 * Objects are structures allocated on the heap. Objects are never allocated
 * statically or on the stack (program initialized objects are exceptions);
 * they must be accessed through special macros and functions only to
 * ensure they are properly garbage-collected.
 *
 * An object has a 'reference count' that is increased or decreased when a
 * pointer to the object is copied or deleted; when the reference count reaches
 * zero there are no references to the object left and it can be removed from
 * the heap.
 *
 * An object has a 'type' that determines what it represents and what kind of
 * data it contains. An object's type is fixed when it's created. Types
 * themselves are represented as objects, and type's 'type' is type object.
 *
 * Objects do not float around in memory; once allocated an object keeps the
 * same size and address (internally, object may be reallocaed before it's used
 * outside). Objects that must hold variable-size data can contain
 * pointers to the variable-size parts of the object. Not all objects of the
 * same type have the same size; but the size cannot change after allocation.
 * (These restrictions are made so a reference to an object can be simply a
 * pointer -- moving an object would required updating all pointers, and
 * changing an object's size would require moving it if there was another object
 * right next to it.)
 *
 * Objects are always accessed through pointers of the type 'COObject *'.
 * It's a structure that only contains the reference count and the type pointer.
 * The actual memory allocated for an object contains other data that can only
 * be accessed after casting the pointer to a pointer to longer structure type.
 * This longer type must start with the reference count and type pointer; the
 * macro COObject_HEAD should be used for this (to accommodate for future
 * changes). The implementation of a particular object type can cast the object
 * pointer to the proper type and back.
 */

#include "compat.h"
#include "mem.h"

// initial segment of every object
#define COObject_HEAD COObject co_base
#define COObject_HEAD_INIT(type_ref)    \
    { 1, type_ref }

// initial segment of every variable-size object
#define COVarObject_HEAD COVarObject co_base;
#define COVarObject_HEAD_INIT(type_ref, size) \
    { COObject_HEAD_INIT(type_ref), size }

// types declared here cuz these is circular reference
typedef struct _COTypeObject COTypeObject;

typedef struct _COObject {
    unsigned int co_refcnt;
    COTypeObject *co_type;
} COObject;

/* for variable-size objects */
typedef struct _COVarObject {
    COObject_HEAD;
    /* we use signed integral type instead of size_t here because sometimes we
     * need negative size, eg. int object */
    ssize_t co_size;
} COVarObject;

#define CO_TYPE(co)     (((COObject *)(co))->co_type)
#define CO_REFCNT(co)     (((COObject *)(co))->co_refcnt)
#define CO_SIZE(co)     (((COVarObject *)(co))->co_size)
#define COObject_Init(co, typeobj)          \
    ( CO_TYPE(co) = (typeobj), CO_REFCNT(co) = 1, (co))
#define COVarObject_Init(co, typeobj, size) \
    ( CO_TYPE(co) = (typeobj), CO_REFCNT(co) = 1, CO_SIZE(co) = size, (co))

COObject _CO_None;              // Don't use this directly, using following one instead!
#define CO_None         (&_CO_None)

#define CO_INCREF(co)   (((COObject *)co)->co_refcnt++)
#define CO_DECREF(co)   \
    do {                \
        if (--((COObject *)co)->co_refcnt == 0) COMem_FREE(co);    \
    } while (0)

/* Macros to used in case the object pointer may be NULL: */
#define CO_XINCREF(co)  do { if ((co) == NULL) ; else CO_INCREF(co); } while (0)
#define CO_XDECREF(co)  do { if ((co) == NULL) ; else CO_DECREF(co); } while (0)

/* COObject_VAR_SIZE returns the number of bytes allocated for a variable-size
 * object with n items. The value is rounded up to the closest multiple of
 * sizeof(void *), in order to ensure that pointer fields at the end of the
 * object are correctly aligned for the platform.
 *
 * Note that there is no memory wastage in doing this, as malloc has to return
 * (at worst) pointer-aligned memory anyway.
 */
#define COObject_VAR_SIZE(typeobj, n)       \
    (size_t) (                              \
        ( (typeobj)->tp_basicsize +         \
          (n) * (typeobj)->tp_itemsize  +   \
          (sizeof(void*)  - 1)              \
        ) & ~(sizeof(void*) - 1)            \
    )

COObject *_COObject_New(COTypeObject *);
COVarObject *_COVarObject_New(COTypeObject *, size_t);

#define COObject_New(type, typeobj)         \
    ((type *)_COObject_New((typeobj)))
#define COVarObject_New(type, typeobj, n)   \
    ((type *)_COVarObject_New((typeobj), (n)))

void COObject_dump(COObject *co);
long COObject_hash(COObject *co);
void COObject_print(COObject *co);
COObject *COObject_repr(COObject *o);

/* Object Interfaces */
typedef COObject *(*unaryfunc)(COObject *);
typedef COObject *(*binaryfunc)(COObject *, COObject *);

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

#endif
