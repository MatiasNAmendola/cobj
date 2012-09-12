#include "../cobj.h"

static COObject *
module_repr(COModuleObject *this)
{
    COObject *s;
    s = COStr_FromFormat("<module>");
    return s;
}

static void
module_dealloc(COModuleObject *this)
{
    COObject_Mem_FREE(this);
}

COTypeObject COModule_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "module",
    sizeof(COModuleObject),
    0,
    0,
    0,                           /* tp_new                  */
    (deallocfunc)module_dealloc, /* tp_dealloc              */
    (reprfunc)module_repr,       /* tp_repr                 */
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
