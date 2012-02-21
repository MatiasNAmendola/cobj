#ifndef DSTRING_H
#define DSTRING_H
/*
 * A dynamic string library.
 *
 * DString is meant to be used with all the usual C string and memory APIs.
 */

#include "co.h"

typedef char *dstring;

struct dstringhdr {
    int len;
    int capacity;
    char buf[];
};

dstring dstring_new(const char *init);
dstring dstring_newl(const void *init, size_t initlen);
size_t dstring_len(const dstring s);
void dstring_free(dstring s);
size_t dstring_avail(const dstring s);
dstring dstring_copy(dstring s, char *t);
dstring dstring_copyl(dstring s, void *t, size_t len);
dstring dstring_cat(dstring s, char *t);
dstring dstring_catl(dstring s, char *t, size_t len);
dstring dstring_getline(FILE * fp);
dstring dstring_readfd(int fd);
dstring dstring_readfile(const char *path);

#endif
