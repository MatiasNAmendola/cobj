#include "co.h"

void
usage(const char *err)
{
    fprintf(stderr, "usage: %s\n", err);
    exit(129);
}

static void
report(const char *prefix, const char *err, va_list params)
{
    char msg[1024];

    vsnprintf(msg, sizeof(msg), err, params);
    fprintf(stderr, "%s%s\n", prefix, msg);
}

void
coerror(const char *err, ...)
{
    va_list params;

    va_start(params, err);
    report("fatal: ", err, params);
    va_end(params);
    exit(128);
}
