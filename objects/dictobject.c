#include "../co.h"

static COObject *dummy = NULL;

static COObject *
dict_repr(CODictObject *this)
{
    COObject *result = NULL;
    COObject *pieces = NULL;
    COObject *colon = NULL;
    COObject *s;
    COObject *key = NULL;
    COObject *item = NULL;
    COObject *tmpobj;

    if (this->nNumOfElements == 0) {
        result = COStr_FromString("{}");
        goto Done;
    }

    pieces = COList_New(0);
    if (!pieces)
        goto Done;

    colon = COStr_FromString(": ");
    if (!colon)
        goto Done;

    DictBucket *tmp = this->pCursor;    // backup current cursor
    this->pCursor = this->pListHead;
    while (CODict_Next((COObject *)this, &key, &item) == 0) {
        int status;
        s = COObject_repr(key);
        COStr_Concat(&s, colon);
        COStr_ConcatAndDel(&s, COObject_repr(item));
        if (s == NULL)
            goto Done;
        status = COList_Append(pieces, s);
        CO_DECREF(s);
        if (status < 0)
            goto Done;
    }
    this->pCursor = tmp;

    s = COStr_FromString("{");
    if (!s)
        goto Done;
    tmpobj = COList_GET_ITEM(pieces, 0);
    COStr_ConcatAndDel(&s, tmpobj);
    COList_SET_ITEM(pieces, 0, s);
    if (!s)
        goto Done;

    s = COStr_FromString("}");
    if (!s)
        goto Done;
    tmpobj = COList_GET_ITEM(pieces, COList_GET_SIZE(pieces) - 1);
    COStr_ConcatAndDel(&tmpobj, s);
    COList_SET_ITEM(pieces, COList_GET_SIZE(pieces) - 1, tmpobj);
    if (!tmpobj)
        goto Done;

    s = COStr_FromString(", ");
    if (!s)
        goto Done;

    result = COStr_Join(s, pieces);
    CO_DECREF(s);

Done:
    CO_XDECREF(pieces);
    CO_XDECREF(colon);
    return result;
}

static void
dict_dealloc(CODictObject *this)
{
    DictBucket *pCursor, *tmp;

    pCursor = this->pListHead;
    while (pCursor) {
        CO_XDECREF(pCursor->pKey);
        CO_XDECREF(pCursor->pItem);
        tmp = pCursor;
        pCursor = pCursor->pListNext;
        COMem_FREE(tmp);
    }

    COMem_FREE(this->arBuckets);
    COMem_FREE(this);
}

static COObject *
dict_subscript(COListObject *this, COObject *index)
{
    COObject *x = CODict_GetItem((COObject *)this, index);
    if (!x) {
        COErr_SetObject(COException_KeyError, index);
        return NULL;
    }
    return x;
}

static COMappingInterface mapping_interface = {
    (lenfunc)CODict_Size,
    (binaryfunc)dict_subscript,
};

COTypeObject CODict_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "dict",
    sizeof(CODictObject),
    0,
    (deallocfunc)dict_dealloc,  /* tp_dealloc */
    (reprfunc)dict_repr,        /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_int_interface */
    &mapping_interface,         /* tp_mapping_interface */
};

int
_dict_rehash(CODictObject *this)
{
    DictBucket *p;

    unsigned int nIndex;

    memset(this->arBuckets, 0, (this->nTableMask + 1) * sizeof(DictBucket *));
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

    if (((this->nTableMask + 1) << 1) > 0) {    // double the table size
        t = (DictBucket **)COMem_REALLOC(this->arBuckets,
                                         ((this->nTableMask + 1) << 1) *
                                         sizeof(DictBucket *));
        this->arBuckets = t;
        this->nTableMask = this->nTableMask << 1;
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
    unsigned long h;
    unsigned int nIndex;

    DictBucket *p;

    h = COObject_Hash(key);
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
            int cmp = COObject_CompareBool(p->pKey, key, Cmp_EQ);
            if (cmp <= 0)
                return NULL;
            if (cmp == 1)
                return p;
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
    unsigned long h = COObject_Hash(key);
    if (h == -1)
        return -1;
    p = (DictBucket *)COMem_MALLOC(sizeof(DictBucket));
    p->pKey = key;
    p->pItem = item;
    p->h = h;

    unsigned int nIndex = h & this->nTableMask;

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

    if (this->nNumOfElements > (this->nTableMask + 1)) {
        _dict_do_resize(this);
    }

    CO_INCREF(key);
    CO_INCREF(item);
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

    CO_DECREF(p->pItem);
    p->pItem = item;
    CO_INCREF(item);

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
           (((CODictObject *)this)->nTableMask + 1) * sizeof(DictBucket *));
    ((CODictObject *)this)->pListHead = NULL;
    ((CODictObject *)this)->pListTail = NULL;
    ((CODictObject *)this)->nNumOfElements = 0;
}

size_t
CODict_Size(COObject *this)
{
    return ((CODictObject *)this)->nNumOfElements;
}

COObject *
CODict_GetItemString(COObject *this, const char *key)
{
    COObject *kv, *o;
    kv = COStr_FromString(key);
    if (!kv)
        return NULL;
    o = CODict_GetItem(this, kv);
    CO_DECREF(kv);
    return o;
}

int
CODict_SetItemString(COObject *this, const char *key, COObject *item)
{
    COObject *kv;
    int err;
    kv = COStr_FromString(key);
    if (!kv)
        return -1;
    err = CODict_SetItem(this, kv, item);
    CO_DECREF(kv);
    return err;
}

int
CODict_DelItemString(COObject *this, const char *key)
{
    COObject *kv;
    int err;
    kv = COStr_FromString(key);
    if (!kv)
        return -1;
    err = CODict_DelItem(this, kv);
    CO_DECREF(kv);
    return err;
}
