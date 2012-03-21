#ifndef OBJECTS_FILEOBJECT_H
#define OBJECTS_FILEOBJECT_H
/**
 * File object interface
 */

#include "../object.h"

typedef struct _COFileObject {
    COObject_HEAD;
    FILE *f_fp;
    COObject *f_name;
    COObject *f_mode;
} COFileObject;

COTypeObject COFile_Type;

#define COFile_Check(co) (CO_TYPE(co) == &COFile_Type)

COObject *COFile_FromFile(FILE *fp, char *, char *, int (*close
COObject *COFile_FromFd(int fd);
COObject *COFile_GetLine(COObject *this);
int COFile_WriteString(COObject *this, const char *s);
int COFile_AsFd(COObject *this);

#endif
