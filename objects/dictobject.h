#ifndef OBJECTS_DICTOBJECT_H
#define OBJECTS_DICTOBJECT_H
/**
 * Dictinary object type
 *
 * This object can map str/int objects to any objects.
 */

#include "../co-compat.h"
#include "../object.h"

typedef ulong (*dict_hash_func_t) (const char *arKey, uint nKeyLen);

typedef struct _DictBucket {
    COObject *pKey;
    COObject *pItem;
    ulong h;

    // global dllist
    struct _DictBucket *pListNext;
    struct _DictBucket *pListLast;

    // bucket dllist
    struct _DictBucket *pNext;
    struct _DictBucket *pLast;
} DictBucket;

typedef struct _CODictObject {
    COObject_HEAD;
    uint nTableSize;
    uint nTableMask;
    uint nNumOfElements;
    DictBucket *pCursor;        /* for iteration */
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

#define CODict_Check(co) (CO_TYPE(co) == &CODict_Type)

COObject *CODict_New(void);
void CODict_Clear(COObject *this);
COObject *CODict_GetItem(COObject *this, COObject *key);
int CODict_SetItem(COObject *this, COObject *key, COObject *item);
int CODict_DelItem(COObject *this, COObject *key);
size_t CODict_Size(COObject *this);
int CODict_Current(COObject *this, COObject **key, COObject **item);
int CODict_Next(COObject *this, COObject **key, COObject **item);
void CODict_Rewind(COObject *this);
COObject *CODict_Keys(COObject *this);
COObject *CODict_Values(COObject *this);
int CODict_Contains(COObject *this, COObject *key);

#endif
