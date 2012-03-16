#ifndef OBJECTS_DICTOBJECT_H
#define OBJECTS_DICTOBJECT_H
/**
 * Dictinary object type -- mapping from hashable object to object
 */

#include "../object.h"
#include "../co-compat.h"

#define HASH_UPDATE         (1<<0)
#define HASH_INSERT         (1<<1)
#define HASH_INSERT_NEXT    (1<<2)

typedef ulong (*dict_hash_func_t) (const char *arKey, uint nKeyLen);

typedef struct _DictBucket {
    ulong h;
    uint nKeyLen;
    void *pData;
    struct _DictBucket *pListNext;
    struct _DictBucket *pListLast;
    struct _DictBucket *pNext;
    struct _DictBucket *pLast;
    char arKey[1];              /* Must be last element */
} DictBucket;

typedef struct _CODictObject {
    COObject_HEAD;
    uint nTableSize;
    uint nTableMask;
    uint nNumOfElements;
    ulong nNextFreeElement;
    DictBucket *pCursor;        /* Used for element traversal */
    DictBucket *pListHead;
    DictBucket *pListTail;
    DictBucket **arBuckets;
} CODictObject;

/* 
 * The minimu size of dictinary. 
 * This suffices for the majority of dicts, to avoid additional malloc.
 */

#define CODict_MINSIZE 8

COTypeObject CODict_Type;

COObject *CODict_New(void);
void CODict_Clear(COObject *this);
COObject *CODict_GetItem(COObject *this, COObject *key);
int CODict_SetItem(COObject *this, COObject *key, COObject *item);
int CODict_DelItem(COObject *this, COObject *key);
size_t CODict_Size(COObject *this);
// TODO
COObject *CODict_Keys(COObject *this);
COObject *CODict_Values(COObject *this);
int CODict_Contains(COObject *this, COObject *key);

#endif
