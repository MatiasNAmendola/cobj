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
    0,                          /* tp_hash */
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

    arKey = ((COStrObject *)key)->co_sval;
    nKeyLen = CO_SIZE(key);
    pData = item;

    if (nKeyLen <= 0) {
        return false;
    }

    h = COObject_hash(key);
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

                p->pData = pData;
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
    p->pData = pData;
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
    COObject_Init(dict, &CODict_Type);

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

    h = COObject_hash(key);
    nIndex = h & ((CODictObject *)this)->nTableMask;

    p = ((CODictObject *)this)->arBuckets[nIndex];

    while (p != NULL) {
        if ((p->h == h) && (p->nKeyLen == CO_SIZE(key))) {
            if (!memcmp(p->arKey, ((COStrObject *)key)->co_sval, CO_SIZE(key))) {
                return p->pData;
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

    h = COObject_hash(key);
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
