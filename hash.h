#ifndef CO_HASH_H
#define CO_HASH_H

#include "co.h"

#define HASH_UPDATE         (1<<0)
#define HASH_INSERT         (1<<1)
#define HASH_INSERT_NEXT    (1<<2)

typedef ulong (*hash_func_t) (const char *arKey, uint nKeyLen);

typedef struct bucket {
    ulong h;
    uint nKeyLen;
    void *pData;
    struct bucket *pListNext;
    struct bucket *pListLast;
    struct bucket *pNext;
    struct bucket *pLast;
    char arKey[1];              /* Must be last element */
} Bucket;

typedef struct hashTable {
    uint nTableSize;
    uint nTableMask;
    uint nNumOfElements;
    ulong nNextFreeElement;
    Bucket *pCursor;            /* Used for element traversal */
    Bucket *pListHead;
    Bucket *pListTail;
    Bucket **arBuckets;
} HashTable;

extern bool co_hash_init(HashTable *ht, uint nSize, hash_func_t pHashFunction);
extern bool co_hash_rehash(HashTable *ht);
extern void co_hash_destory(HashTable *ht);
extern void co_hash_clean(HashTable *ht);
extern bool co_hash_find(const HashTable *ht, const char *arKey, uint nKeyLen, void **pData);
extern bool _co_hash_insert_or_update(HashTable *ht, const char *arKey,
                                      uint nKeyLen, void *pData, uint nDataSize, int flag);

#define co_hash_insert(ht, arKey, nKeyLen, pData, nDataSize) \
        _co_hash_insert_or_update(ht, arKey, nKeyLen, pData, nDataSize, HASH_INSERT)
#define co_hash_update(ht, arKey, nKeyLen, pData, nDataSize) \
        _co_hash_insert_or_update(ht, arKey, nKeyLen, pData, nDataSize, HASH_UPDATE)

extern bool co_hash_del(HashTable *ht, const char *arKey, uint nKeyLen);

#if CO_DEBUG
void co_hash_display(const HashTable *ht);
#endif

/* Symbol Table */
static inline bool
co_symtable_update(HashTable *ht, const char *arKey, uint nKeyLen, void *pData, uint nDateSize)
{
    return co_hash_update(ht, arKey, nKeyLen, pData, nDateSize);
}

static inline bool
co_symtable_del(HashTable *ht, const char *arKey, uint nKeyLen)
{
    return co_hash_del(ht, arKey, nKeyLen);
}

static inline bool
co_symtable_find(HashTable *ht, const char *arKey, uint nKeyLen, void **pData)
{
    return co_hash_find(ht, arKey, nKeyLen, pData);
}

/*
 * Borrowed From PHP/Zend Engine
 *
 * DJBX33A (Daniel J. Bernstein, Times 33 with Addition)
 *
 * This is Daniel J. Bernstein's popular `times 33' hash function as
 * posted by him years ago on comp.lang.c. It basically uses a function
 * like ``hash(i) = hash(i-1) * 33 + str[i]''. This is one of the best
 * known hash functions for strings. Because it is both computed very
 * fast and distributes very well.
 *
 * The magic of number 33, i.e. why it works better than many other
 * constants, prime or not, has never been adequately explained by
 * anyone. So I try an explanation: if one experimentally tests all
 * multipliers between 1 and 256 (as RSE did now) one detects that even
 * numbers are not useable at all. The remaining 128 odd numbers
 * (except for the number 1) work more or less all equally well. They
 * all distribute in an acceptable way and this way fill a hash table
 * with an average percent of approx. 86%.
 *
 * If one compares the Chi^2 values of the variants, the number 33 not
 * even has the best value. But the number 33 and a few other equally
 * good numbers like 17, 31, 63, 127 and 129 have nevertheless a great
 * advantage to the remaining numbers in the large set of possible
 * multipliers: their multiply operation can be replaced by a faster
 * operation based on just one shift plus either a single addition
 * or subtraction operation. And because a hash function has to both
 * distribute good _and_ has to be very fast to compute, those few
 * numbers should be preferred and seems to be the reason why Daniel J.
 * Bernstein also preferred it.
 *
 *
 *                  -- Ralf S. Engelschall <rse@engelschall.com>
 */

static inline ulong
co_inline_hash_func(const char *arKey, uint nKeyLen)
{
    register ulong hash = 5381;

    /* variant with the hash unrolled eight times */
    for (; nKeyLen >= 8; nKeyLen -= 8) {
        hash = ((hash << 5) + hash) + *arKey++;
        hash = ((hash << 5) + hash) + *arKey++;
        hash = ((hash << 5) + hash) + *arKey++;
        hash = ((hash << 5) + hash) + *arKey++;
        hash = ((hash << 5) + hash) + *arKey++;
        hash = ((hash << 5) + hash) + *arKey++;
        hash = ((hash << 5) + hash) + *arKey++;
        hash = ((hash << 5) + hash) + *arKey++;
    }
    switch (nKeyLen) {
    case 7:
        hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 6:
        hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 5:
        hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 4:
        hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 3:
        hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 2:
        hash = ((hash << 5) + hash) + *arKey++; /* fallthrough... */
    case 1:
        hash = ((hash << 5) + hash) + *arKey++;
        break;
    case 0:
        break;
    default:
        break;
    }
    return hash;
}
#endif
