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

static FILE *
find_module(char *name, COObject *path)
{
    ssize_t i;
    ssize_t npath = COList_GET_SIZE(path);

    char buf[PATH_MAX];
    size_t len, namelen;
    namelen = strlen(name);
    FILE *fp;
    for (i = 0; i < npath; i++) {
        COObject *v = COList_GetItem(path, i);
        if (!v)
            goto error;
        len = COStr_GET_SIZE(v);
        strcpy(buf, COStr_AS_STRING(v));
        buf[len++] = '/';
        strcpy(buf+len, name);
        len += namelen;
        strcpy(buf+len, ".co");
        len += 3;
        buf[len] = '\0';

        fp = fopen(buf, "rb");
        if (fp) {
            break;
        }
    }
    if (!fp) {
        COErr_Format(COException_ImportError, "No module named %.200s", name);
        goto error;
    }

    return fp;

error:
    return NULL;
}

COObject *
module_import(COObject *name)
{
    struct arena *arena = arena_new();
    scanner_init(arena);

    FILE *fp = find_module(COStr_AS_STRING(name), GS(mainthread)->module_search_path);
    if (!fp)
        return NULL;

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
