#include "cobj.h"

const struct module_entry module_table[] = {
    {"base", module_base_init},
    {"gc", module_gc_init},

    /* sentinel */
    {0, 0},
};

COObject *
module_init_builtins(void)
{
    COObject *builtins = CODict_New();
    const struct module_entry *m;
    for (m = module_table; m->initfunc; m++) {
        CODict_SetItemString(builtins, m->name, m->initfunc());
    }
    COObject *base = CODict_GetItemString(builtins, "base");
    if (base) {
        COObject *key;
        COObject *val;
        while (CODict_Next(base, &key, &val) == 0) {
            CODict_SetItem(builtins, key, val);
        }
    }
    return builtins;
}
