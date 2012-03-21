#include "../co.h"

COObject *
COFile_FromFile(FILE *fp, char *name, char *mode, int (*close)(FILE *))
{
    COFileObject *f = COObject_New(COFileObject, &COFile_Type);
}

COObject *
COFile_FromFd(int fd)
{
}

COObject *
COFile_GetLine(COObject *this)
{
}

int
COFile_WriteString(COObject *this, const char *s)
{
}

int
COFile_AsFd(COObject *this)
{
}
