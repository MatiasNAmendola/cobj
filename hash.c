#include "co.h"

#define CONNECT_TO_BUCKET_DLLIST(element, list_head)    \
    (element)->pNext = (list_head);                     \
    (element)->pLast = NULL;                            \
    if ((element)->pNext) {                             \
        (element)->pNext->pLast = (element);            \
    }

#define CONNECT_TO_GLOBAL_DLLIST(element, ht)           \
    (element)->pListLast = (ht)->pListTail;             \
    (ht)->pListTail = (element);                        \
    (element)->pListNext = NULL;                        \
    if ((element)->pListLast != NULL) {                 \
        (element)->pListLast->pListNext = (element);    \
    }                                                   \
    if ((ht)->pListHead == NULL) {                      \
        (ht)->pListHead = (element);                    \
    }                                                   \
    if ((ht)->pCursor == NULL) {                        \
        (ht)->pCursor = (element);                      \
    }

static bool co_hash_do_resize(HashTable *ht);

bool
co_hash_init(HashTable *ht, uint nSize, hash_func_t pHashFunction)
{
    uint i = 3;
    Bucket **tmp;

    memset(ht, 0, sizeof(HashTable));

    if (nSize >= 0x80000000) {
        /* prevent overflow */
        ht->nTableSize = 0x80000000;
    } else {
        while ((1U << i) < nSize) {
            i++;
        }
        ht->nTableSize = 1 << i;
    }
    ht->nTableSize = nSize;

    ht->nTableMask = ht->nTableSize - 1;
    ht->arBuckets = NULL;
    ht->nNumOfElements = 0;
    ht->nNextFreeElement = 0;
    tmp = (Bucket **)xcalloc(ht->nTableSize, sizeof(Bucket *));
    ht->arBuckets = tmp;

    return true;
}

static bool
co_hash_do_resize(HashTable *ht)
{
    Bucket **t;

    if ((ht->nTableSize << 1) > 0) {    // double the table size
        t = (Bucket **)xrealloc(ht->arBuckets,
                                (ht->nTableSize << 1) * sizeof(Bucket *));
        ht->arBuckets = t;
        ht->nTableSize = ht->nTableSize << 1;
        ht->nTableMask = ht->nTableSize - 1;
        co_hash_rehash(ht);
        return true;
    }

    return true;                // can not be larger
}

bool
co_hash_rehash(HashTable *ht)
{
    Bucket *p;

    uint nIndex;

    memset(ht->arBuckets, 0, ht->nTableSize * sizeof(Bucket *));
    p = ht->pListHead;
    while (p != NULL) {
        nIndex = p->h & ht->nTableMask;
        ht->arBuckets[nIndex] = p;
        p = p->pListNext;
    }
    return true;
}

bool
_co_hash_insert_or_update(HashTable *ht, const char *arKey, uint nKeyLen,
                          void *pData, uint nDataSize, int flag)
{
    ulong h;

    uint nIndex;

    Bucket *p;

    if (nKeyLen <= 0) {
        return false;
    }

    h = co_inline_hash_func(arKey, nKeyLen);
    nIndex = h & ht->nTableMask;
    p = ht->arBuckets[nIndex];

    while (p != NULL) {
        if ((p->h == h) && (p->nKeyLen == nKeyLen)) {
            if (!memcmp(p->arKey, arKey, nKeyLen)) {
                /* key equal */

                // cannot add
                if (flag & HASH_INSERT) {
                    return false;
                }
                // update
                if (p->pData == pData) {
                    return false;
                }

                p->pData = xrealloc(p->pData, nDataSize);
                memcpy(p->pData, pData, nDataSize);
                return true;
            }
        }

        /* key not equal */
        p = p->pNext;
    }

    // add
    p = (Bucket *)xmalloc(sizeof(Bucket) - 1 + nKeyLen);
    memcpy(p->arKey, arKey, nKeyLen);
    p->nKeyLen = nKeyLen;

    p->pData = xmalloc(nDataSize);
    memcpy(p->pData, pData, nDataSize);

    p->h = h;

    CONNECT_TO_BUCKET_DLLIST(p, ht->arBuckets[nIndex]);
    CONNECT_TO_GLOBAL_DLLIST(p, ht);
    ht->arBuckets[nIndex] = p;

    ht->nNumOfElements++;

    if (ht->nNumOfElements > ht->nTableSize) {
        co_hash_do_resize(ht);
    }
    return true;
}

void
co_hash_destory(HashTable *ht)
{
    Bucket *p, *q;

    p = ht->pListHead;
    while (p != NULL) {
        q = p;
        free(q->pData);
        free(q);
        p = p->pListNext;
    }
    free(ht->arBuckets);
}

void
co_hash_clean(HashTable *ht)
{
    Bucket *p, *q;

    p = ht->pListHead;
    while (p != NULL) {
        q = p;
        free(q->pData);
        free(q);
        p = p->pListNext;
    }
    memset(ht->arBuckets, 0, ht->nTableSize * sizeof(Bucket *));
    ht->pListHead = NULL;
    ht->pListTail = NULL;
    ht->nNumOfElements = 0;
    ht->nNextFreeElement = 0;
    ht->pCursor = NULL;
}

/*
 * Return true is found and false is not.
 * The pointer to the data is returned in pData
 */
bool
co_hash_find(const HashTable *ht, const char *arKey, uint nKeyLen, void **pData)
{
    ulong h;

    uint nIndex;

    Bucket *p;

    h = co_inline_hash_func(arKey, nKeyLen);
    nIndex = h & ht->nTableMask;

    p = ht->arBuckets[nIndex];

    while (p != NULL) {
        if ((p->h == h) && (p->nKeyLen == nKeyLen)) {
            if (!memcmp(p->arKey, arKey, nKeyLen)) {
                *pData = p->pData;
                return true;
            }
        }
        p = p->pNext;
    }

    return false;
}

bool
co_hash_del(HashTable *ht, const char *arKey, uint nKeyLen)
{
    uint nIndex;

    Bucket *p;

    ulong h;

    h = co_inline_hash_func(arKey, nKeyLen);

    nIndex = h & ht->nTableMask;

    p = ht->arBuckets[nIndex];

    while (p != NULL) {
        if (p->h == h && p->nKeyLen == nKeyLen) {
            if (!memcmp(p->arKey, arKey, nKeyLen)) {    /* Key Equal */
                // remove from bucket dllist
                if (p == ht->arBuckets[nIndex]) {
                    ht->arBuckets[nIndex] = p->pNext;
                } else {
                    p->pLast->pNext = p->pNext;
                }
                if (p->pNext) {
                    p->pNext->pLast = p->pLast;
                }
                // remove from global ddlist
                if (p->pListLast == NULL) {
                    ht->pListHead = p->pListNext;
                } else {
                    p->pListLast->pListNext = p->pListNext;
                }
                if (p->pListNext == NULL) {
                    ht->pListTail = p->pListLast;
                } else {
                    p->pListNext->pListLast = p->pListLast;
                }
                free(p->pData);
                free(p);
                ht->nNumOfElements--;
                return true;
            }
        } else {
            p = p->pNext;
        }
    }
    return false;
}

#if CO_DEBUG
void
co_hash_display(const HashTable *ht)
{
    Bucket *p;

    uint i;

    for (i = 0; i < ht->nTableSize; i++) {
        p = ht->arBuckets[i];
        while (p != NULL) {
            printf("%s <==> 0x%lX\n", p->arKey, p->h);
            p = p->pNext;
        }
    }

    p = ht->pListTail;
    while (p != NULL) {
        while (p != NULL) {
            printf("%s <==> 0x%lX\n", p->arKey, p->h);
            p = p->pListLast;
        }
    }
}
#endif
