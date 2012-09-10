#include "../cobj.h"

#define BUILTIN_FUNC(func) \
    COCFunctionObject _builtin_##func = { \
        COObject_HEAD_INIT(&COCFunction_Type), \
        #func, \
        builtin_##func \
    }


static COObject *
builtin_print(COObject *this, COObject *args)
{
    int i;
    if (args) {
        for (i = 0; i < COTuple_Size(args); i++) {
            if (i > 0)
                printf(" ");
            COObject_Print(COTuple_GET_ITEM(args, i), stdout);
        }
    }
    printf("\n");

    CO_RETURN_NONE;
}


static COObject *
builtin_len(COObject *this, COObject *args)
{
    // TODO check args
    if (COTuple_Size(args) != 1) {
        COErr_BadInternalCall();
        return NULL;
    }
    COObject *o = COTuple_GET_ITEM(args, 0);
    ssize_t s = COObject_Length(o);
    if (COErr_Occurred()) {
        return NULL;
    }
    return COInt_FromLong((long)s);
}

BUILTIN_FUNC(print);
BUILTIN_FUNC(len);

COObject *
module_base_init(void)
{
    COObject *base = CODict_New();
    CODict_SetItemString(base, "print", (COObject *)&_builtin_print);
    CODict_SetItemString(base, "len", (COObject *)&_builtin_len);
    CODict_SetItemString(base, "type", (COObject *)&COType_Type);
    CODict_SetItemString(base, "str", (COObject *)&COStr_Type);
    CODict_SetItemString(base, "range", (COObject *)&CORange_Type);
    CODict_SetItemString(base, "set", (COObject *)&COSet_Type);
    CODict_SetItemString(base, "file", (COObject *)&COFile_Type);
    return base;
}
