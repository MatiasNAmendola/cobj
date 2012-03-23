#ifndef WRAPPER_H
#define WRAPPER_H

void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
void *xcalloc(size_t nmemb, size_t size);
char *xstrndup(const char *str, size_t len);
bool safe_strtol(const char *str, long int *out);

#endif
