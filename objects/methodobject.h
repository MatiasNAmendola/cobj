#ifndef OBJECTS_METHODOBJECT_H
#define OBJECTS_METHODOBJECT_H
/**
 * Method Object.
 *
 * Method object is a callable object that is called on a object.
 */

#include "../object.h"
#include "../compile.h"

typedef struct _COMethodObject {
    COObject_HEAD;
    COObject *m_func;       /* The callable Function/CFunction object. */
    COObject *m_self;       /* The object it is bound to. */
} COMethodObject;

COTypeObject COMethod_Type;

#define COMethod_Check(co) (CO_TYPE(co) == &COMethod_Type)

COObject *COMethod_New(COObject *, COObject *);

#define COMethod_GET_FUNCTION(meth) (((COMethodObject *)meth)->m_func)
#define COMethod_GET_SELF(meth) (((COMethodObject *)meth)->m_self)

#endif
