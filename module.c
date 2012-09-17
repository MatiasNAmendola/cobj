#include "cobj.h"

const struct module_entry module_table[] = {
    {"base", module_base_init, IMPORT_ALL},
    {"gc", module_gc_init, IMPORT_DEFAULT},
    {"io", module_io_init, IMPORT_DEFAULT},

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
        m = mt->initfunc ();
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

COObject *
module_import(COObject *name)
{
    struct arena *arena = arena_new();
    scanner_init(arena);

    FILE *fp = fopen("./examples/module.co", "rb");
    if (fp == NULL) {
        COErr_BadInternalCall();
        return NULL;
    }
    COObject *f = COFile_FromFile(fp, name, COStr_FromString("rb"));
    if (!f)
        return NULL;

    COObject *src = COFile_Read(f, -1);

    scanner_setcode(COBytes_AsString(src));
    COObject *code = compile(arena);

    COObject *globals = CODict_New();
    COObject *func = COFunction_New(code);
    COObject *ret = vm_eval(func, globals);
    if (!ret) {
        if (COErr_Occurred()) {
            COErr_Print();
            return NULL;
        }
    }
    CO_DECREF(func);
    CO_DECREF(code);

    arena_free(arena);

    COObject *module = COModule_New(name);
    COObject *dict = COModule_GetDict(module);
    COObject *key;
    COObject *val;
    while (CODict_Next(globals, &key, &val) == 0) {
        CODict_SetItem(dict, key, val);
    }
    return module;
}
