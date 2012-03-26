#ifndef WRAPPER_H
#define WRAPPER_H

char *xstrndup(const char *str, size_t len);
bool safe_strtol(const char *str, long int *out);

#endif
