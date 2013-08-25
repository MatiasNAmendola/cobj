#ifndef STRTOD_H
#define STRTOD_H

double str2d(const char *str, char **endptr);

/* d2str's flags parameter can be set to 0 or more of: */
#define D2STR_FLAGS_SIGN      0x01      /* always add the sign */
#define D2STR_FLAGS_ADD_DOT_0 0x02      /* if the result is an integer add ".0" */
#define D2STR_FLAGS_ALT       0x04      /* "alternative" formatting, it's format_code specific */

/* d2str's type parameter, if non-NULL, will be set to one of: */
#define D2STR_TYPE_FINITE       0
#define D2STR_TYPE_INFINITE     1
#define D2STR_TYPE_NAN          2

char *d2str(double val, char format_code, int precision, int flags, int *type);

#endif
