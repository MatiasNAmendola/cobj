#include "co.h"

COObject *
COErr_Occurred(void)
{
    return TS(curexc_type);
}

void
COErr_Restore(COObject *type, COObject *value, COObject *traceback)
{
    COObject *oldtype, *oldvalue, *oldtraceback;

    oldtype = TS(curexc_type);
    oldvalue = TS(curexc_value);
    oldtraceback = TS(curexc_traceback);

    TS(curexc_type) = type;
    TS(curexc_value) = value;
    TS(curexc_traceback) = traceback;

    CO_XINCREF(type);
    CO_XINCREF(value);
    CO_XINCREF(traceback);
    CO_XDECREF(oldtype);
    CO_XDECREF(oldvalue);
    CO_XDECREF(oldtraceback);
}

void
COErr_Fetch(COObject **type, COObject **value, COObject **traceback)
{
    *type = TS(curexc_type);
    *value = TS(curexc_value);
    *traceback = TS(curexc_traceback);

    TS(curexc_type) = NULL;
    TS(curexc_value) = NULL;
    TS(curexc_traceback) = NULL;
}

void
COErr_Print(void)
{
    COObject *type, *value, *traceback;

    COErr_Fetch(&type, &value, &traceback);
    fprintf(stderr, "%s: %s\n", ((COTypeObject *)type)->tp_name,
            COStr_AsString(value));
    CO_XDECREF(type);
    CO_XDECREF(value);
    CO_XDECREF(traceback);
}

void
COErr_SetObject(COObject *exception, COObject *value)
{
    /*if (!COException_Check(exception)) { */
    /*COErr_Format(COException_SystemError, "not a exception"); */
    /*return; */
    /*} */
    COErr_Restore(exception, value, NULL);
}

void
COErr_Format(COObject *exception, const char *fmt, ...)
{
    va_list vargs;
    COObject *str;

    va_start(vargs, fmt);

    str = COStr_FromFormatV(fmt, vargs);
    COErr_SetObject(exception, str);
    CO_DECREF(str);

    va_end(vargs);
}

void
COErr_SetString(COObject *exception, const char *str)
{
    COObject *o = COStr_FromString(str);
    COErr_SetObject(exception, o);
    CO_XDECREF(o);
}

void
_COErr_BadInternalCall(const char *filename, int lineno)
{
    COErr_Format(COException_SystemError,
                 "%s:%d: bad argument to internal function", filename, lineno);
}

COObject *
COErr_NoMemory(void)
{
    COErr_SetObject(COException_MemoryError, CO_None);
    return NULL;
}
