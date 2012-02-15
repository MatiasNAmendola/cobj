#ifndef WRAPPER_H
#define WRAPPER_H

extern void *xmalloc(size_t size);
extern void *xrealloc(void *ptr, size_t size);
extern void *xcalloc(size_t nmemb, size_t size);
extern char *xstrndup(const char *str, size_t len);
extern bool safe_strtol(const char *str, long int *out);

#endif
