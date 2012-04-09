#include "../co.h"

static COObject *dummy = NULL;

static COObject *
dict_repr(CODictObject *this)
{
#ifdef CO_DEBUG
    COObject *key;
    COObject *item;
    while (CODict_Next((COObject *)this, &key, &item) == 0) {
        COObject_dump(key);
        COObject_dump(item);
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
    0,                          /* tp_compare */
    0,                          /* tp_int_interface */
};

int
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
    return 0;
}

static int
_dict_do_resize(CODictObject *this)
{
    DictBucket **t;

    if ((this->nTableSize << 1) > 0) {  // double the table size
        t = (DictBucket **)COMem_REALLOC(this->arBuckets,
                                         (this->nTableSize << 1) *
                                         sizeof(DictBucket *));
        this->arBuckets = t;
        this->nTableSize = this->nTableSize << 1;
        this->nTableMask = this->nTableSize - 1;
        _dict_rehash(this);
        return 0;
    }

    return -1;                  // can not be larger
}

/* 
 * Dict lookup function.
 * It only support str/int object now.
 */
static DictBucket *
_dict_lookup(CODictObject *this, COObject *key)
{
    ulong h;
    uint nIndex;

    DictBucket *p;

    h = COObject_hash(key);
    if (h == -1)
        return NULL;

    nIndex = h & this->nTableMask;
    p = this->arBuckets[nIndex];

    while (p != NULL) {
        if (p->h == h) {
            if (p->pKey == dummy) {
                // TODO free slots
                continue;
            }
            if (COStr_Check(key)) {
                if (!memcmp
                    (((COStrObject *)p->pKey)->co_sval,
                     ((COStrObject *)key)->co_sval, CO_SIZE(key))) {
                    return p;
                }
            } else if (key == CO_None) {
                return p;
            } else if (COInt_Check(key)) {
                int cmp = COObject_CompareBool(p->pKey, key, Cmp_EQ);
                if (cmp <= 0)
                    return NULL; 
                if (cmp == 1)
                    return p;
            } else {
                // TODO
                assert(0);
            }
        }

        /* key not equal */
        p = p->pNext;
    }

    return NULL;
}

static int
_dict_insert(CODictObject *this, COObject *key, COObject *item)
{
    DictBucket *p;
    ulong h = COObject_hash(key);
    if (h == -1)
        return -1;
    p = (DictBucket *)COMem_MALLOC(sizeof(DictBucket));
    p->pKey = key;
    p->pItem = item;
    p->h = h;

    uint nIndex = h & this->nTableMask;

    // connect to bucket dllist
    p->pNext = this->arBuckets[nIndex];
    p->pLast = NULL;
    if (p->pNext) {
        p->pNext->pLast = p;
    }
    // connect to global dllist
    p->pListLast = this->pListTail;
    this->pListTail = p;
    p->pListNext = NULL;
    if (p->pListLast != NULL) {
        p->pListLast->pListNext = p;
    }
    if (this->pListHead == NULL) {
        this->pListHead = p;
    }
    if (this->pCursor == NULL) {
        this->pCursor = p;
    }

    this->arBuckets[nIndex] = p;
    this->nNumOfElements++;

    if (this->nNumOfElements > this->nTableSize) {
        _dict_do_resize(this);
    }

    return 0;
}

void
_dict_destory(CODictObject *this)
{
    DictBucket *p, *q;

    p = this->pListHead;
    while (p != NULL) {
        q = p;
        free(q->pItem);
        free(q);
        p = p->pListNext;
    }
    free(this->arBuckets);
}

COObject *
CODict_New(void)
{
    if (dummy == NULL) {
        dummy = COStr_FromString("<dummy key>");
    }
    DictBucket **tmp;
    CODictObject *dict = COObject_New(CODictObject, &CODict_Type);
    dict->pCursor = NULL;
    dict->pListHead = NULL;
    dict->pListTail = NULL;

    dict->nTableMask = CODict_MINSIZE - 1;
    dict->nNumOfElements = 0;
    tmp =
        (DictBucket **)COMem_MALLOC((dict->nTableMask + 1) *
                                    sizeof(DictBucket *));
    memset(tmp, 0, (dict->nTableMask + 1) * sizeof(DictBucket *));
    dict->arBuckets = tmp;

    return (COObject *)dict;
}

/*
 * Return NULL if the key is not present.
 */
COObject *
CODict_GetItem(COObject *this, COObject *key)
{
    DictBucket *p = _dict_lookup((CODictObject *)this, key);
    if (!p) {
        return NULL;
    }
    return p->pItem;
}

int
CODict_SetItem(COObject *this, COObject *key, COObject *item)
{
    DictBucket *p = _dict_lookup((CODictObject *)this, key);
    if (!p) {
        return _dict_insert((CODictObject *)this, key, item);
    }

    p->pItem = item;

    return 0;
}

int
CODict_DelItem(COObject *this, COObject *key)
{
    DictBucket *p = _dict_lookup((CODictObject *)this, key);
    if (!p) {
        return -1;
    }

    p->pKey = dummy;

    ((CODictObject *)this)->nNumOfElements--;

    return 0;
}

int
CODict_Current(COObject *this, COObject **key, COObject **item)
{
    if (((CODictObject *)this)->pCursor == NULL) {
        return -1;
    }
    *key = ((CODictObject *)this)->pCursor->pKey;
    *item = ((CODictObject *)this)->pCursor->pItem;
    return 0;
}

int
CODict_Next(COObject *this, COObject **key, COObject **item)
{
    if (CODict_Current(this, key, item) != 0) {
        return -1;
    }

    ((CODictObject *)this)->pCursor =
        ((CODictObject *)this)->pCursor->pListNext;
    return 0;
}

void
CODict_Rewind(COObject *this)
{
    ((CODictObject *)this)->pCursor = ((CODictObject *)this)->pListHead;
}

void
CODict_Clear(COObject *this)
{
    DictBucket *p, *q;

    p = ((CODictObject *)this)->pListHead;
    while (p != NULL) {
        q = p;
        free(q->pItem);
        free(q);
        p = p->pListNext;
    }
    memset(((CODictObject *)this)->arBuckets, 0,
           ((CODictObject *)this)->nTableSize * sizeof(DictBucket *));
    ((CODictObject *)this)->pListHead = NULL;
    ((CODictObject *)this)->pListTail = NULL;
    ((CODictObject *)this)->nNumOfElements = 0;
}

size_t
CODict_Size(COObject *this)
{
    return ((CODictObject *)this)->nNumOfElements;
}
