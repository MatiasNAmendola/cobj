#include "cobj.h"

static int
_case_insensitive_match(const char *s, const char *t)
{
    while (*t && tolower(*s) == *t) {
        s++;
        t++;
    }
    return *t ? 0 : 1;
}

double
_parse_inf_or_nam(const char *str, char **endptr)
{
    double retval = -1;
    const char *s;
    int negate = 0;

    s = str;

    if (*s == '-') {
        negate = 1;
        s++;
    } else if (*s == '+') {
        s++;
    }

    if (_case_insensitive_match(s, "inf")) {
        s += 3;
        if (_case_insensitive_match(s, "inity"))
            s += 5;

        retval = negate ? -INFINITY : INFINITY;
    } else if (_case_insensitive_match(s, "nan")) {
        s += 3;
        retval = negate ? -NAN : NAN;
    } else {
        s = str;
        retval = -1.0;
    }

    *endptr = (char *)s;
    return retval;
}

/* 
 * `strtod` convers a null-terminated byte string s to a float. The string
 * should not have leading or trailling whitespace. The conversion is
 * independent of the current locale.
 *
 * If endptr is NULL, try to convert the whole string. Set errno to -1 and
 * returns -1.0 f the string is not a valid representation of a float-point
 * number.
 *
 * If endptr is non-NULL, try to convert as much of the string as possible. If
 * no initial segment of the string is the valid representation of a
 * floating-pointer number than *endptr is set to point to the begining of the
 * string, -1.0 is returned and and errno is set to -1.
 *
 * Use system strtod; since strtod is locale aware, we may have to first fix the
 * decimal separator.
 */
double
str2d(const char *str, char **endptr)
{
    char *fail_pos = NULL;
    double val = -1.0;

    struct lconv *locale_data;
    const char *decimal_point;
    size_t decimal_point_len;
    const char *p, *decimal_point_pos = NULL;
    const char *end = NULL;
    const char *digits_pos = NULL;
    int negate = 0;

    assert(str != NULL);

    locale_data = localeconv();
    decimal_point = locale_data->decimal_point;
    decimal_point_len = strlen(decimal_point);

    assert(decimal_point_len != 0);

    /* parse infinities and nans */
    val = _parse_inf_or_nam(str, &fail_pos);
    if (fail_pos != str) {
        return val;
    }

    /* We process the optional sign manually, then pass the remainder to the
     * system strtod. This ensures that the result of an underflow has the
     * correct sign. */
    p = str;
    if (*p == '-') {
        negate = 1;
        p++;
    } else if (*p == '+') {
        p++;
    }

    if (!isdigit(*p) && *p != '.')
        goto invalid_string;

    digits_pos = p;
    if (decimal_point[0] != '.' || decimal_point[1] != 0) {
        while (isdigit(*p))
            p++;

        if (*p == '.') {
            decimal_point_pos = p++;

            while (isdigit(*p))
                p++;

            if (*p == 'e' || *p == 'E')
                p++;
            if (*p == '+' || *p == '-')
                p++;

            while (isdigit(*p))
                p++;

            end = p;
        } else if (strncmp(p, decimal_point, decimal_point_len) == 0)
            goto invalid_string;
    }

    if (decimal_point_pos) {
        /* Create a copy of the input, with the '.' converted to the
         * locale-specific decimal point */
        // TODO
        assert(0);
    } else {
        val = strtod(digits_pos, &fail_pos);
    }

    if (fail_pos == digits_pos)
        goto invalid_string;

    if (negate && fail_pos != str)
        val = -val;

    if (endptr) {
        *endptr = fail_pos;
    }
    return val;

invalid_string:
    if (endptr) {
        *endptr = (char *)str;
    }
    errno = EINVAL;
    return -1.0;
}
