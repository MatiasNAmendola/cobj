/**
 * IO Module.
 */

#include "../cobj.h"

COObject *
module_io_init(void)
{
    COObject *name = COStr_FromString("io");
    COObject *io = COModule_New(name);
    CO_DECREF(name);
    COObject *dict = COModule_GetDict(io);

    return io;
}
