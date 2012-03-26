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
    COObject_dump(type);
    COObject_dump(value);
}
