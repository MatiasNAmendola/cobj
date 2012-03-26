/*
 * Various trival helper wrappers around standard functions
 */
#include "co.h"

char *
xstrndup(const char *str, size_t len)
{
    char *p;

    p = (char *)COMem_MALLOC(len + 1);
    memcpy(p, str, len);
    p[len] = '\0';

    return p;
}

bool
safe_strtol(const char *str, long int *out)
{
    assert(out != NULL);
    errno = 0;
    *out = 0;
    char *endptr;

    long int l = strtol(str, &endptr, 10);

    if (errno == ERANGE)
        return false;
    if (isspace(*endptr) || (*endptr == '\0' && endptr != str)) {
        *out = l;
        return true;
    }
    return false;
}

static char bad_path[] = "/bad-path/";

static char *cleanup_path(char *path)
{
    /* Clean it up */
    if (!memcmp(path, "./", 2)) {
        path += 2;
        while (*path == '/')
            path++;
    }
    return path;
}

char *mksnpath(char *buf, size_t n, const char *fmt, ...)
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

