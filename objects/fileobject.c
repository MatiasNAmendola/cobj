#include "../co.h"

#if BUFSIZ < 8192
# define SMALL_CHUNK 8192
#else
# define SMALL_CHUNK BUFSIZ
#endif

static COObject *
file_repr(COExceptionObject *this)
{
    return COStr_FromString("<Exception>");
}

static void
file_dealloc(COFileObject *this)
{
    CO_XDECREF(this->f_name);
    CO_XDECREF(this->f_mode);
    COObject_Mem_FREE(this);
}

COTypeObject COFile_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "file",
    sizeof(COFileObject),
    0,
    0,
    0,                          /* tp_new */
    (deallocfunc)file_dealloc,  /* tp_dealloc */
    (reprfunc)file_repr,        /* tp_repr */
    0,                          /* tp_print */
    0,                          /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_traverse */
    0,                          /* tp_clear */
    0,                          /* tp_call */
    0,                          /* tp_int_interface */
    0,                          /* tp_mapping_interface */
};

COObject *
COFile_FromFile(FILE *fp, char *name, char *mode, int (*close) (FILE *))
{
    COFileObject *f = COObject_NEW(COFileObject, &COFile_Type);
    if (f == NULL) {
        return NULL;
    }
    COObject *f_name = COStr_FromString(name);
    if (f_name == NULL) {
        return NULL;
    }
    COObject *f_mode = COStr_FromString(mode);
    if (f_mode == NULL) {
        return NULL;
    }
    f->f_fp = fp;
    f->f_name = f_name;
    f->f_mode = f_mode;
    f->f_close = close;
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

    return o;
}

int
COFile_WriteString(COObject *this, const char *s)
{
    return 0;
}
