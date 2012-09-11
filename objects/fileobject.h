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

    char *f_buf;                /* Allocated readahead buffer. */
    char *f_bufend;             /* Points after last occupied position. */
    char *f_bufptr;             /* Current buffer position. */
    int f_univ_newline;         /* Handle any newline convention. */
    int f_newlinetypes;         /* Handle any newline convention. */
    int f_skipnextif;           /* Skip next \n. */
} COFileObject;

COTypeObject COFile_Type;

#define COFile_Check(co) (CO_TYPE(co) == &COFile_Type)

COObject *COFile_FromFile(FILE *fp, COObject *name, COObject *mode);
FILE *COFile_AsFile(COObject *this);
COObject *COFile_GetLine(COObject *this);
COObject *COFile_Read(COObject *this, int n);
int COFile_WriteString(COObject *this, const char *s);

#endif
