#include "../cobj.h"
/**
 * Base module.
 *
 * Basic functions that do misc things.
 */

#define BASE_FUNC(func) \
    COCFunctionObject _base_##func = { \
        COObject_HEAD_INIT(&COCFunction_Type), \
        #func, \
        base_##func \
    }

static COObject *
base_print(COObject *args)
{
    int i;
    if (args) {
        for (i = 0; i < COTuple_GET_SIZE(args); i++) {
            if (i > 0)
                printf(" ");
            if (COObject_Print(COTuple_GET_ITEM(args, i), stdout) != 0) {
                return NULL;
            }
        }
    }
    printf("\n");

    CO_RETURN_NONE;
}

static COObject *
base_len(COObject *args)
{
    if (COTuple_GET_SIZE(args) != 1) {
        COErr_BadInternalCall();
        return NULL;
    }
    COObject *o = COTuple_GET_ITEM(args, 0);
    ssize_t s = COSequence_Length(o);
    if (COErr_Occurred()) {
        return NULL;
    }
    return COInt_FromLong((long)s);
}

BASE_FUNC(print);
BASE_FUNC(len);

COObject *
module_base_init(void)
{
    COObject *name = COStr_FromString("base");
    COObject *base = COModule_New(name);
    CO_DECREF(name);
    COObject *dict = COModule_GetDict(base);

    CODict_SetItemString(dict, "file", (COObject *)&COFile_Type);
    CODict_SetItemString(dict, "len", (COObject *)&_base_len);
    CODict_SetItemString(dict, "object", (COObject *)&COObject_Type);
    CODict_SetItemString(dict, "print", (COObject *)&_base_print);
    CODict_SetItemString(dict, "range", (COObject *)&CORange_Type);
    CODict_SetItemString(dict, "bool", (COObject *)&COBool_Type);
    CODict_SetItemString(dict, "int", (COObject *)&COInt_Type);
    CODict_SetItemString(dict, "set", (COObject *)&COSet_Type);
    CODict_SetItemString(dict, "str", (COObject *)&COStr_Type);
    CODict_SetItemString(dict, "type", (COObject *)&COType_Type);
    CODict_SetItemString(dict, "Exception", COException);
    CODict_SetItemString(dict, "AttributeError", COException_AttributeError);
    CODict_SetItemString(dict, "ImportError", COException_ImportError);
    CODict_SetItemString(dict, "IndexError", COException_IndexError);
    CODict_SetItemString(dict, "KeyError", COException_KeyError);
    CODict_SetItemString(dict, "MemoryError", COException_MemoryError);
    CODict_SetItemString(dict, "NameError", COException_NameError);
    CODict_SetItemString(dict, "OverflowError", COException_OverflowError);
    CODict_SetItemString(dict, "SystemError", COException_SystemError);
    CODict_SetItemString(dict, "TypeError", COException_TypeError);
    CODict_SetItemString(dict, "NotImplementedError",
                         COException_NotImplementedError);
    CODict_SetItemString(dict, "ValueError", COException_ValueError);
    CODict_SetItemString(dict, "ZeroDivisionError",
                         COException_ZeroDivisionError);

    return base;
}
