#include "../co.h"

static COObject *
dict_repr(CODictObject *this)
{
#ifdef CO_DEBUG
    uint i;
    DictBucket *p;
    for (i = 0; i < this->nTableSize; i++) {
        p = this->arBuckets[i];
        while (p != NULL) {
            printf("%s <==> 0x%lX\n", p->arKey, p->h);
            p = p->pNext;
        }
    }

    p = this->pListTail;
    while (p != NULL) {
        while (p != NULL) {
            printf("%s <==> 0x%lX\n", p->arKey, p->h);
            p = p->pListLast;
        }
    }
#endif

    return COStr_FromFormat("<dict 'size: %d'>", this->nNumOfElements);
}

COTypeObject CODict_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "dict",
    sizeof(CODictObject),
    0,
    (reprfunc)dict_repr,        /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
};

#define HASH_UPDATE         (1<<0)
#define HASH_INSERT         (1<<1)

#define CONNECT_TO_BUCKET_DLLIST(element, list_head)    \
    (element)->pNext = (list_head);                     \
    (element)->pLast = NULL;                            \
    if ((element)->pNext) {                             \
        (element)->pNext->pLast = (element);            \
    }

#define CONNECT_TO_GLOBAL_DLLIST(element, this)           \
    (element)->pListLast = (this)->pListTail;             \
    (this)->pListTail = (element);                        \
    (element)->pListNext = NULL;                        \
    if ((element)->pListLast != NULL) {                 \
        (element)->pListLast->pListNext = (element);    \
    }                                                   \
    if ((this)->pListHead == NULL) {                      \
        (this)->pListHead = (element);                    \
    }                                                   \
    if ((this)->pCursor == NULL) {                        \
        (this)->pCursor = (element);                      \
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
dict_inline_hash_func(const char *arKey, uint nKeyLen)
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

bool
_dict_rehash(CODictObject *this)
{
    DictBucket *p;

    uint nIndex;

    memset(this->arBuckets, 0, this->nTableSize * sizeof(DictBucket *));
    p = this->pListHead;
    while (p != NULL) {
        nIndex = p->h & this->nTableMask;
        this->arBuckets[nIndex] = p;
        p = p->pListNext;
    }
    return true;
}

static bool
_dict_do_resize(CODictObject *this)
{
    DictBucket **t;

    if ((this->nTableSize << 1) > 0) {  // double the table size
        t = (DictBucket **)xrealloc(this->arBuckets,
                                    (this->nTableSize << 1) *
                                    sizeof(DictBucket *));
        this->arBuckets = t;
        this->nTableSize = this->nTableSize << 1;
        this->nTableMask = this->nTableSize - 1;
        _dict_rehash(this);
        return true;
    }

    return true;                // can not be larger
}

bool
_dict_insert_or_update(CODictObject *this, COObject *key, COObject *item,
                       int flag)
{
    ulong h;

    uint nIndex;

    DictBucket *p;

    const char *arKey;
    uint nKeyLen;
    void *pData;
    uint nDataSize;

    arKey = ((COStrObject *)key)->co_sval;
    nKeyLen = CO_SIZE(key);
    pData = &item;
    nDataSize = sizeof(COObject *);

    if (nKeyLen <= 0) {
        return false;
    }

    h = dict_inline_hash_func(arKey, nKeyLen);
    nIndex = h & this->nTableMask;
    p = this->arBuckets[nIndex];

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
    p = (DictBucket *)xmalloc(sizeof(DictBucket) - 1 + nKeyLen);
    memcpy(p->arKey, arKey, nKeyLen);
    p->nKeyLen = nKeyLen;

    p->pData = xmalloc(nDataSize);
    memcpy(p->pData, pData, nDataSize);

    p->h = h;

    CONNECT_TO_BUCKET_DLLIST(p, this->arBuckets[nIndex]);
    CONNECT_TO_GLOBAL_DLLIST(p, this);
    this->arBuckets[nIndex] = p;

    this->nNumOfElements++;

    if (this->nNumOfElements > this->nTableSize) {
        _dict_do_resize(this);
    }
    return true;
}

void
_dict_destory(CODictObject *this)
{
    DictBucket *p, *q;

    p = this->pListHead;
    while (p != NULL) {
        q = p;
        free(q->pData);
        free(q);
        p = p->pListNext;
    }
    free(this->arBuckets);
}

COObject *
CODict_New(void)
{
    DictBucket **tmp;
    CODictObject *dict = xmalloc(sizeof(CODictObject));
    memset(dict, 0, sizeof(CODictObject));
    CO_INIT(dict, &CODict_Type);

    dict->nTableSize = CODict_MINSIZE;

    dict->nTableMask = dict->nTableSize - 1;
    dict->arBuckets = NULL;
    dict->nNumOfElements = 0;
    dict->nNextFreeElement = 0;
    tmp = (DictBucket **)xcalloc(dict->nTableSize, sizeof(DictBucket *));
    dict->arBuckets = tmp;

    return (COObject *)dict;
}

/*
 * Return NULL if the key is not present.
 */
COObject *
CODict_GetItem(COObject *this, COObject *key)
{
    ulong h;

    uint nIndex;

    DictBucket *p;

    // TODO only support string object now, uses object hash value instead
    h = dict_inline_hash_func(((COStrObject *)key)->co_sval, CO_SIZE(key));
    nIndex = h & ((CODictObject *)this)->nTableMask;

    p = ((CODictObject *)this)->arBuckets[nIndex];

    while (p != NULL) {
        if ((p->h == h) && (p->nKeyLen == CO_SIZE(key))) {
            if (!memcmp(p->arKey, ((COStrObject *)key)->co_sval, CO_SIZE(key))) {
                return *(COObject **)p->pData;
            }
        }
        p = p->pNext;
    }

    return NULL;
}

int
CODict_SetItem(COObject *this, COObject *key, COObject *item)
{
    return _dict_insert_or_update((CODictObject *)this, key, item, HASH_UPDATE);
}

int
CODict_DelItem(COObject *_this, COObject *key)
{
    CODictObject *this = (CODictObject *)_this;
    uint nIndex;

    DictBucket *p;

    ulong h;

    const char *arKey;
    uint nKeyLen;
    arKey = ((COStrObject *)key)->co_sval;
    nKeyLen = CO_SIZE(key);

    h = dict_inline_hash_func(arKey, nKeyLen);

    nIndex = h & this->nTableMask;

    p = this->arBuckets[nIndex];

    while (p != NULL) {
        if (p->h == h && p->nKeyLen == nKeyLen) {
            if (!memcmp(p->arKey, arKey, nKeyLen)) {    /* Key Equal */
                // remove from bucket dllist
                if (p == this->arBuckets[nIndex]) {
                    this->arBuckets[nIndex] = p->pNext;
                } else {
                    p->pLast->pNext = p->pNext;
                }
                if (p->pNext) {
                    p->pNext->pLast = p->pLast;
                }
                // remove from global ddlist
                if (p->pListLast == NULL) {
                    this->pListHead = p->pListNext;
                } else {
                    p->pListLast->pListNext = p->pListNext;
                }
                if (p->pListNext == NULL) {
                    this->pListTail = p->pListLast;
                } else {
                    p->pListNext->pListLast = p->pListLast;
                }
                free(p->pData);
                free(p);
                this->nNumOfElements--;
                return 0;
            }
        } else {
            p = p->pNext;
        }
    }
    return -1;
}

void
CODict_Clear(COObject *_this)
{
    CODictObject *this = (CODictObject *)_this;
    DictBucket *p, *q;

    p = this->pListHead;
    while (p != NULL) {
        q = p;
        free(q->pData);
        free(q);
        p = p->pListNext;
    }
    memset(this->arBuckets, 0, this->nTableSize * sizeof(DictBucket *));
    this->pListHead = NULL;
    this->pListTail = NULL;
    this->nNumOfElements = 0;
    this->nNextFreeElement = 0;
    this->pCursor = NULL;
}

size_t
CODict_Size(COObject *this)
{
    return ((CODictObject *)this)->nNumOfElements;
}
