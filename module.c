#include "cobj.h"

const struct module_entry module_table[] = {
    {"base", module_base_init, IMPORT_ALL},
    {"gc", module_gc_init, IMPORT_DEFAULT},

    /* sentinel */
    {0, 0},
};

COObject *
module_init(void)
{
    /**
     * import * from base
     * import gc
     */
    COObject *builtins = CODict_New();
    COObject *m = NULL;
    const struct module_entry *mt;
    for (mt = module_table; mt->initfunc; mt++) {
        m = mt->initfunc();
        if (mt->flag & IMPORT_ALL) {
            COObject *dict = COModule_GetDict(m);
            COObject *key;
            COObject *val;
            while (CODict_Next(dict, &key, &val) == 0) {
                CODict_SetItem(builtins, key, val);
            }
        } else {
            CODict_SetItemString(builtins, mt->name, m);
        }
    }
    return builtins;
}
