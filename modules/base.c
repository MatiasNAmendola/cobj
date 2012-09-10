#include "../cobj.h"

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

COCFunctionObject _CO_Builtin_print = {
    COObject_HEAD_INIT(&COCFunction_Type),
    "print",
    builtin_print,
};


COObject *
module_base_init(void)
{
    COObject *base = CODict_New();
    CODict_SetItemString(base, "print", (COObject *)&_CO_Builtin_print);
    CODict_SetItemString(base, "type", (COObject *)&COType_Type);
    CODict_SetItemString(base, "str", (COObject *)&COStr_Type);
    CODict_SetItemString(base, "range", (COObject *)&CORange_Type);
    CODict_SetItemString(base, "set", (COObject *)&COSet_Type);
    CODict_SetItemString(base, "file", (COObject *)&COFile_Type);
    return base;
}
