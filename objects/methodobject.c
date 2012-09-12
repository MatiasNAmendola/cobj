#include "../cobj.h"

static COObject *
method_repr(COMethodObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<method>");
    return s;
}

static void
method_dealloc(COMethodObject *this)
{
    CO_XDECREF(this->m_func);
    CO_XDECREF(this->m_self);
    COObject_Mem_FREE(this);
}

COTypeObject COMethod_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "method",
    sizeof(COMethodObject),
    0,
    0,
    0,                           /* tp_new                  */
    (deallocfunc)method_dealloc, /* tp_dealloc              */
    (reprfunc)method_repr,       /* tp_repr                 */
    0,                           /* tp_print                */
    0,                           /* tp_hash                 */
    0,                           /* tp_compare              */
    0,                           /* tp_traverse             */
    0,                           /* tp_clear                */
    0,                           /* tp_call                 */
    0,                           /* tp_iter                 */
    0,                           /* tp_iternext             */
    0,                           /* tp_arithmetic_interface */
    0,                           /* tp_mapping_interface    */
    0,                           /* tp_sequence_interface   */
};

COObject *
COMethod_New(COObject *func, COObject *self)
{
    COMethodObject *this = COObject_NEW(COMethodObject, &COMethod_Type);
    this->m_func = func;
    CO_INCREF(func);
    this->m_self = self;
    CO_INCREF(self);
    return (COObject *)this;
}
