#include "../cobj.h"

#define SMALL_CHUNK 8192

/* Flags for f_newlinetypes. */
#define NEWLINE_UNKNOWN 0
#define NEWLINE_CR      1
#define NEWLINE_LF      2
#define NEWLINE_CRLF    4

static COObject *
file_repr(COFileObject *this)
{
    return COStr_FromFormat("<file '%s', mode '%s', at %p>",
                            COStr_AS_STRING(this->f_name), COStr_AsString(this->f_mode), this);
}

static void
file_dealloc(COFileObject *this)
{
    fclose(this->f_fp);
    CO_DECREF(this->f_name);
    CO_DECREF(this->f_mode);
    COObject_Mem_FREE(this);
}

static COObject *
file_new(COTypeObject *type, COObject *args)
{
    COObject *name = NULL;
    if (!COObject_ParseArgs(args, &name, NULL))
        return NULL;

    const char *mode = "r";
    FILE *fp = fopen(COStr_AsString(name), mode);
    if (!fp) {
        COErr_SetString(COException_SystemError, "Failed to open file");
        return NULL;
    }
    COObject *f_mode = COStr_FromString(mode);

    COObject *f = COFile_FromFile(fp, name, f_mode);
    CO_DECREF(f_mode);
    return f;
}

static COObject *
file_read(COObject *args)
{
    COObject *this = COTuple_GET_ITEM(args, 0);
    return COFile_Read((COObject *)this, -1);
}

static COMethodDef file_methods[] = {
    {"read", (COCFunction)file_read, 0},
    {NULL, NULL}
};

static void
drop_readahead(COFileObject *f)
{
    if (f->f_buf != NULL) {
        COObject_Mem_FREE(f->f_buf);
        f->f_buf = NULL;
    }
}

/**
 * It's a fread variation that understands all of \r, \n and \r\n conventions.
 */
size_t
universal_newline_fread(char *buf, size_t n, FILE *stream, COFileObject *f)
{
    char *dst = buf;
    int newlinetypes, skipnextlf;

    if (!f->f_univ_newline) {
        return fread(buf, 1, n, stream);
    }

    newlinetypes = f->f_newlinetypes;
    skipnextlf = f->f_skipnextif;

    /* Invariant: n is the number of bytes remaining to be filled in the
     * buffer.
     */
    while (n) {
        size_t nread;
        int shortread;
        char *src = dst;

        nread = fread(dst, 1, n, stream);
        if (nread == 0)
            break;

        n -= nread;
        shortread = n != 0;

        while (nread--) {
            char c = *src++;
            if (c == '\r') {
                *dst++ = '\n';
                skipnextlf = 1;
            } else if (skipnextlf && c == '\n') {
                skipnextlf = 0;
                newlinetypes |= NEWLINE_CRLF;
                ++n;
            } else {
                if (c == '\n')
                    newlinetypes |= NEWLINE_LF;
                else if (skipnextlf)
                    newlinetypes |= NEWLINE_CR;
                *dst++ = c;
                skipnextlf = 0;
            }
        }
        if (shortread) {
            // If this is EOF, update type flags.
            if (skipnextlf && feof(stream))
                newlinetypes |= NEWLINE_CR;
            break;
        }

    }

    f->f_newlinetypes = newlinetypes;
    f->f_skipnextif = skipnextlf;
    return dst - buf;
}

/**
 * Make sure that file has a readahead buffer with at leat one byte (unless EOF)
 * and no more than bufsize. Returns nagative value on error, will set
 * MemoryError if buffsize bytes cannot be allocated.
 */
static int
__readahead(COFileObject *f, int bufsize)
{
    ssize_t chunksize;
    if (f->f_buf != NULL) {
        if (f->f_bufend - f->f_bufptr >= 1)
            return 0;
        else
            drop_readahead(f);
    }

    if ((f->f_buf = (char *)COObject_Mem_MALLOC(bufsize)) == NULL) {
        COErr_NoMemory();
        return -1;
    }

    errno = 0;
    chunksize = universal_newline_fread(f->f_buf, bufsize, f->f_fp, f);
    if (chunksize == 0) {
        if (ferror(f->f_fp)) {
            clearerr(f->f_fp);
            drop_readahead(f);
            return -1;
        }
    }
    f->f_bufptr = f->f_buf;
    f->f_bufend = f->f_buf + chunksize;
    return 0;
}

/**
 * The returned string will start with 'skip' uninitialized bytes followed by
 * the remainder of the line.
 */
static COStrObject *
readahead_get_line_skip(COFileObject *f, int skip, int bufsize)
{
    char *buf;
    char *bufptr;
    ssize_t len;

    if (f->f_buf == NULL)
        if (__readahead(f, bufsize) < 0)
            return NULL;

    len = f->f_bufend - f->f_bufptr;
    if (len == 0)
        return (COStrObject *)COStr_FromStringN(NULL, skip);

    COStrObject *s;
    bufptr = (char *)memchr(f->f_bufptr, '\n', len);
    if (bufptr != NULL) {
        bufptr++;
        len = bufptr - f->f_bufptr;
        s = (COStrObject *)COStr_FromStringN(NULL, skip + len);
        if (s == NULL)
            return NULL;
        memcpy(COStr_AS_STRING(s) + skip, f->f_bufptr, len);
        f->f_bufptr = bufptr;
        if (bufptr == f->f_bufend)
            drop_readahead(f);
    } else {
        bufptr = f->f_bufptr;
        buf = f->f_buf;
        f->f_buf = NULL;
        assert(skip + len < INT_MAX);
        s = readahead_get_line_skip(f, (int)(skip + len),
                                    bufsize + (bufsize >> 2));
        if (!s) {
            COObject_Mem_FREE(buf);
            return NULL;
        }
        memcpy(COStr_AS_STRING(s) + skip, bufptr, len);
        COObject_Mem_FREE(buf);
    }
    return s;
}

static COObject *
file_iternext(COFileObject *f)
{
    COStrObject *l;
    l = readahead_get_line_skip(f, 0, 8192);
    if (l == NULL || CO_SIZE(l) == 0) {
        CO_XDECREF(l);
        return NULL;
    }
    return (COObject *)l;
}

COTypeObject COFile_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "file",
    sizeof(COFileObject),
    0,
    0,
    (newfunc)file_new,          /* tp_new               */
    (deallocfunc)file_dealloc,  /* tp_dealloc           */
    (reprfunc)file_repr,        /* tp_repr              */
    0,                          /* tp_print             */
    0,                          /* tp_hash              */
    0,                          /* tp_compare           */
    0,                          /* tp_traverse          */
    0,                          /* tp_clear             */
    0,                          /* tp_call              */
    (getiterfunc)COObject_GetSelf,      /* tp_iter              */
    (iternextfunc)file_iternext,        /* tp_iternext          */
    0,                          /* tp_arithmetic_interface     */
    0,                          /* tp_mapping_interface */
    0,                          /* tp_sequence_interface */
    0,
    0,
    file_methods,
    0,
};

COObject *
COFile_FromFile(FILE *fp, COObject *name, COObject *mode)
{
    COFileObject *f = COObject_NEW(COFileObject, &COFile_Type);
    if (f == NULL) {
        return NULL;
    }
    if (name == NULL) {
        return NULL;
    }
    if (mode == NULL) {
        return NULL;
    }
    f->f_fp = fp;
    f->f_name = name;
    CO_INCREF(name);
    f->f_mode = mode;
    CO_INCREF(mode);
    f->f_buf = NULL;
    f->f_bufend = NULL;
    f->f_bufptr = NULL;
    f->f_univ_newline = 0;
    f->f_newlinetypes = NEWLINE_UNKNOWN;
    f->f_skipnextif = 0;
    return (COObject *)f;
}

FILE *
COFile_AsFile(COObject *this)
{
    return ((COFileObject *)this)->f_fp;
}

COObject *
COFile_GetLine(COObject *this)
{
    // TODO
    return NULL;
}

COObject *
COFile_Read(COObject *this, int n)
{
    COFileObject *f = (COFileObject *)this;
    size_t buffersize;
    COObject *o;
    size_t bytesread = 0;

    if (n < 0) {
        buffersize = SMALL_CHUNK;
    } else {
        buffersize = n;
    }

    o = COBytes_FromStringN(NULL, buffersize);
    char *o_start = COBytes_AsString(o);
    if (o == NULL)
        return NULL;

    for (;;) {
        size_t read = fread(o_start + bytesread, 1, buffersize - bytesread,
                            f->f_fp);
        if (read == 0) {
            if (!ferror(f->f_fp))
                break;
            clearerr(f->f_fp);
            return NULL;
        }
        bytesread += read;
        if (bytesread < buffersize) {
            clearerr(f->f_fp);
            break;
        }
        if (n < 0) {
            buffersize += SMALL_CHUNK;
            if (COBytes_Resize(o, buffersize) < 0)
                return NULL;
            continue;           // read more
        } else {
            break;
        }
    }
    o_start[bytesread] = '\0';

    COBytes_Resize(o, bytesread);
    return o;
}

int
COFile_WriteString(COObject *this, const char *s)
{
    // TODO
    return 0;
}
