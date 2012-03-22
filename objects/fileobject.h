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
    int (*f_close) (FILE *);
} COFileObject;

COTypeObject COFile_Type;

#define COFile_Check(co) (CO_TYPE(co) == &COFile_Type)

COObject *COFile_FromFile(FILE *fp, char *name, char *mode,
                          int (*close) (FILE *));
FILE *COFile_AsFile(COObject *this);
COObject *COFile_GetLine(COObject *this);
COObject *COFile_Read(COObject *this, int n);
int COFile_WriteString(COObject *this, const char *s);

#endif
