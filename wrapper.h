#ifndef WRAPPER_H
#define WRAPPER_H

/* PATH */
char *mksnpath(char *buf, size_t n, const char *fmt, ...);
/* ! PATH */

int prefixcmp(const char *str, const char *prefix);
int suffixcmp(const char *str, const char *suffix);

void usage(const char *err);
void error(const char *err, ...);
#endif
