#include "co.h"

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
