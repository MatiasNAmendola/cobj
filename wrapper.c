/*
 * Various trival helper wrappers around standard functions
 */
#include "co.h"

static char bad_path[] = "/bad-path/";

static char *
cleanup_path(char *path)
{
    /* Clean it up */
    if (!memcmp(path, "./", 2)) {
        path += 2;
        while (*path == '/')
            path++;
    }
    return path;
}

char *
mksnpath(char *buf, size_t n, const char *fmt, ...)
{
    va_list args;
    unsigned len;

    va_start(args, fmt);
    len = vsnprintf(buf, n, fmt, args);
    va_end(args);
    if (len >= n) {
        strlcpy(buf, bad_path, n);
        return buf;
    }
    return cleanup_path(buf);
}

int
prefixcmp(const char *str, const char *prefix)
{
    for (;; str++, prefix++)
        if (!*prefix)
            return 0;
        else if (*str != *prefix)
            return (unsigned char)*prefix - (unsigned char)*str;
}

int
suffixcmp(const char *str, const char *suffix)
{
    int len = strlen(str), suflen = strlen(suffix);
    if (len < suflen)
        return -1;
    else
        return strcmp(str + len - suflen, suffix);
}

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
error(const char *err, ...)
{
    va_list params;

    va_start(params, err);
    report("fatal: ", err, params);
    va_end(params);
    exit(128);
}
