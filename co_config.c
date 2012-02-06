/*
 * The information manager
 */

#include "co_config.h"

#define MAXNAME 256

static FILE *config_file;

static const char *config_file_name;

static int config_linenr;

static int config_file_eof;

static int
get_next_char(void)
{
    int c;

    FILE *f;

    c = '\n';
    if ((f = config_file) != NULL) {
        c = fgetc(f);
        if (c == '\r') {
            /* DOS-like systems */
            c = fgetc(f);
            if (c != '\n') {
                ungetc(c, f);
                c = '\r';
            }
        }
        if (c == '\n')
            config_linenr++;
        if (c == EOF) {
            config_file_eof = 1;
            c = '\n';
        }
    }
    return c;
}

static char *
parse_value(void)
{
    static char value[1024];

    int quote = 0, comment = 0, len = 0, space = 0;

    for (;;) {
        int c = get_next_char();

        if (len >= sizeof(value) - 1)
            return NULL;

        if (c == '\n') {
            if (quote)
                return NULL;
            value[len] = '\0';
            return value;
        }

        if (comment)
            continue;

        if (isspace(c) && !quote) {
            space = 1;
            continue;
        }

        if (!quote) {
            if (c == ';' || c == '#') {
                comment = 1;
                continue;
            }
        }

        if (space) {
            if (len)
                value[len++] = ' ';
            space = 0;
        }

        if (c == '\\') {
            c = get_next_char();
            switch (c) {
            case '\n':
                continue;
            case 't':
                c = '\t';
                break;
            case 'b':
                c = '\b';
                break;
            case 'n':
                c = '\n';
                break;
                /* Some characters escape as themselves */
            case '\\':
            case '"':
                break;
                /* Reject unknown escape sequences */
            default:
                return NULL;
            }
            value[len++] = c;
            continue;
        }

        if (c == '"') {
            quote = 1 - quote;
            continue;
        }

        value[len++] = c;
    }
}

static inline int
iskeychar(int c)
{
    return isalnum(c) || c == '_';
}

static int
get_value(config_fn_t fn, void *data, char *name, unsigned int len)
{
    int c;

    char *value;

    /* Get the full name */
    for (;;) {
        c = get_next_char();
        if (config_file_eof)
            break;
        if (!iskeychar(c))
            break;
        name[len++] = tolower(c);
        if (len >= MAXNAME)
            return -1;
    }
    name[len] = 0;
    while (c == ' ' || c == '\t')
        c = get_next_char();

    value = NULL;
    if (c != '\n') {
        if (c != '=')
            return -1;
        value = parse_value();
        if (!value)
            return -1;
    }
    return fn(name, value, data);
}

static int
get_base_var(char *name)
{
    int baselen = 0;

    for (;;) {
        int c = get_next_char();

        if (config_file_eof)
            return -1;
        if (c == ']')
            return baselen;
        if (!iskeychar(c))
            return -1;
        if (baselen > MAXNAME / 2)
            return -1;
        name[baselen++] = tolower(c);
    }
}

static int
parse_file(config_fn_t fn, void *data)
{
    int comment = 0;

    int baselen = 0;

    static char var[MAXNAME];

    /* U+FEFF Byte Order Mark in UTF8 */
    static const unsigned char *utf8_bom = (unsigned char *)"\xef\xbb\xbf";

    const unsigned char *bomptf = utf8_bom;

    for (;;) {
        int c = get_next_char();

        if (bomptf && *bomptf) {
            /* We are at the file beginning; skip UTF8-encoded BOM if present.
             * Sane editors won't put this in on their own, but e.g. Windows
             * Notepad will do it happily.
             */
            if ((unsigned char)c == *bomptf) {
                /* skip */
                bomptf++;
                continue;
            } else {
                /* Do not tolerate partial BOM. */
                if (bomptf != utf8_bom)
                    break;
                /* No BOM at file beginning. */
                bomptf = NULL;
            }
        }

        if (c == '\n') {
            if (config_file_eof)
                return 0;
            comment = 0;
            continue;
        }

        if (comment || isspace(c))
            continue;
        if (c == '#' || c == ';') {
            comment = 1;
            continue;
        }

        if (c == '[') {
            baselen = get_base_var(var);
            if (baselen <= 0)
                break;
            var[baselen++] = '.';
            var[baselen] = 0;
            continue;
        }

        if (!isalpha(c))
            break;
        var[baselen] = tolower(c);
        if (get_value(fn, data, var, baselen + 1) < 0)
            break;
    }
    die("Bad config file line %d in %s", config_linenr, config_file_name);
}
