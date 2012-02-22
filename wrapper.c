/*
 * Various trival helper wrappers around standard functions
 */
#include "co.h"

void *
xmalloc(size_t size)
{
    void *ret = malloc(size);

    if (!ret) {
        coerror("Out of memory, malloc failed");
    }

    return ret;
}

void *
xrealloc(void *ptr, size_t size)
{
    void *ret = realloc(ptr, size);

    if (!ret) {
        coerror("Out of memory, realloc failed");
    }

    return ret;
}

void *
xcalloc(size_t nmemb, size_t size)
{
    void *ret = calloc(nmemb, size);

    if (!ret) {
        coerror("Out of memory, realloc failed");
    }

    return ret;
}

char *
xstrndup(const char *str, size_t len)
{
    char *p;

    p = (char *)xmalloc(len + 1);
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
