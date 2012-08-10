#ifndef OBJECT_H
#define OBJECT_H
/*
 * Object interface (Object, Type, Value)
 *
 * !!! Derived from Python 3 Include/object.h, Object/object.c.
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

void _CO_NegativeRefCnt(const char *fname, int lineno, COObject *co);

#define CO_TYPE(co)     (((COObject *)(co))->co_type)
#define CO_REFCNT(co)     (((COObject *)(co))->co_refcnt)
#define CO_SIZE(co)     (((COVarObject *)(co))->co_size)

#define COObject_INIT(co, typeobj)          \
    ( CO_TYPE(co) = (typeobj), CO_REFCNT(co) = 1, (co))
#define COVarObject_INIT(co, typeobj, size) \
    ( CO_TYPE(co) = (typeobj), CO_REFCNT(co) = 1, CO_SIZE(co) = size, (co))

#define CO_INCREF(co)   (((COObject *)co)->co_refcnt++)
#define CO_DECREF(co)   \
    do {                \
        if (--((COObject *)co)->co_refcnt != 0) { \
            if (((COObject *)co)->co_refcnt < 0) { \
                _CO_NegativeRefCnt(__FILE__, __LINE__, (COObject *)(co));  \
            } \
        } else { \
            (CO_TYPE(co)->tp_dealloc)((COObject *)(co)); \
        } \
    } while (0)

/* Macros to used in case the object pointer may be NULL: */
#define CO_XINCREF(co)  do { if ((co) == NULL) ; else CO_INCREF(co); } while (0)
#define CO_XDECREF(co)  do { if ((co) == NULL) ; else CO_DECREF(co); } while (0)

/* Safely decref `o` and set `o` to NULL, especially useful in tp_clear and
 * tp_dealloc implementations.
 */
#define CO_CLEAR(o)                                 \
    do {                                            \
        if (o) {                                    \
            COObject *tmp = (COObject *)(o);        \
            (o) = NULL;                             \
            CO_DECREF(tmp);                         \
        }                                           \
    } while (0);

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

#define COObject_NEW(type, typeobj)         \
    ((type *)COObject_New((typeobj)))
#define COVarObject_NEW(type, typeobj, n)   \
    ((type *)COVarObject_New((typeobj), (n)))

#define COObject_GC_NEW(type, typeobj)          \
    ((type *)COObject_GC_New((typeobj)))
#define COVarObject_GC_NEW(type, typeobj, n)    \
    ((type *)COVarObject_GC_New((typeobj), (n)))

#define COObject_IS_GC(o)   (COType_HasFeature(CO_TYPE(o), COType_FLAG_GC))

COObject *COObject_New(COTypeObject *);
COObject *COVarObject_New(COTypeObject *tp, ssize_t n);
void COObject_Dump(COObject *co);
long COObject_Hash(COObject *co);
int COObject_Print(COObject *o, FILE *fp);
COObject *COObject_Repr(COObject *o);
COObject *COObject_Compare(COObject *a, COObject *b, int op);
int COObject_CompareBool(COObject *a, COObject *b, int op);
int COObject_IsTrue(COObject *o);
int COObject_ParseArgs(COObject *args, ...);
COObject *COObject_Str(COObject *o);
COObject *COObject_Call(COObject *func, COObject *args);
COObject *COObject_GetIter(COObject *o);
COObject *COObject_GetSelf(COObject *o);

#define CO_RETURN_NONE return CO_INCREF(CO_None), CO_None

/* Macro to help write tp_traverse functions.
 */
#define CO_VISIT(o)                                     \
    do {                                                \
        if (o) {                                        \
            int ret = visit((COObject *)(o), arg);      \
            if (ret)                                    \
                return ret;                             \
        }                                               \
    } while (0);

#endif
