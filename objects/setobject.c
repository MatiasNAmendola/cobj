#include "../cobj.h"

/* Object used as dummy key to fill deleted entries. */
static COObject *dummy = NULL;

/* Must be >= 1. */
#define PERTURB_SHIFT 5

#define INIT_NONZERO_SET_SLOTS(so) do {                         \
    (so)->table = (so)->smalltable;                             \
    (so)->mask = COSet_MINSIZE - 1;                             \
    (so)->hash = -1;                                            \
    } while(0)

#define EMPTY_TO_MINSIZE(so) do {                               \
    memset((so)->smalltable, 0, sizeof((so)->smalltable));      \
    (so)->used = (so)->fill = 0;                                \
    INIT_NONZERO_SET_SLOTS(so);                                 \
    } while(0)

/*static COObject **/
/*set_repr(COSetObject *this)*/
/*{*/
/*if (this->used == 0) {*/
/*return COStr_FromString("set()");*/
/*}*/
/*COObject *s;*/
/*s = COStr_FromString("set(");*/
/*int i;*/
/*for (i = 0; i < this->used; i++) {*/
/*COObject *co = COSet_GetItem(this, i);*/
/*if (i != 0)*/
/*COStr_ConcatAndDel(&s, COStr_FromString(", "));*/
/*COStr_ConcatAndDel(&s, COObject_Repr(co));*/
/*}*/
/*COStr_ConcatAndDel(&s, COStr_FromString(")"));*/
/*return s;*/
/*}*/

static void
set_dealloc(COSetObject *this)
{

}

/**
 * The basic lookup function used by all operations.
 * This is based on Algorithm D from Knuth Vol. 3, Sec. 6.4.
 * Open addressing is preferred over chaining since the link overhead for
 * chaining would be substantial (100% with typical malloc overhead).
 */
static struct setentry *
set_lookkey(COSetObject *so, COObject *key, long hash)
{
    size_t i;
    size_t perturb;
    struct setentry *freeslot;
    size_t mask = so->mask;
    struct setentry *table = so->table;
    struct setentry *entry;
    int cmp;
    COObject *startkey;

    i = (size_t) hash & mask;
    entry = &table[i];
    if (entry->key == NULL || entry->key == key)
        return entry;

    if (entry->key == dummy) {
        freeslot = entry;
    } else {
        if (entry->hash == hash) {
            startkey = entry->key;
            cmp = COObject_CompareBool(startkey, key, Cmp_EQ);
            if (cmp < 0)
                return NULL;
            if (cmp > 0)
                return entry;
        }
        freeslot = NULL;
    }

    for (perturb = hash; ; perturb >>= PERTURB_SHIFT) {
        i = (i << 2) + i + perturb + 1;
        entry = &table[i & mask];
        if (entry->key == NULL) {
            if (freeslot != NULL)
                entry = freeslot;
            break;
        }

        if (entry->key == key)
            break;

        if (entry->hash == hash && entry->key != dummy) {
            startkey = entry->key;
            cmp = COObject_CompareBool(startkey, key, Cmp_EQ);
            if (cmp < 0)
                return NULL;
            if (cmp > 0)
                break;
        } else if (entry->key == dummy && freeslot == NULL) {
            freeslot = entry;
        }
    }

    return entry;
}

static int
set_merge(COSetObject *so, COObject *other)
{

    return 0;
}

static int
set_update_internal(COSetObject *so, COObject *other)
{

    return 0;
}

static COObject *
make_new_set(COTypeObject *type, COObject *iterable)
{
    if (!dummy) {
        dummy = COStr_FromString("<dummy key>");
        if (!dummy)
            return NULL;
    }

    COSetObject *so = NULL;
    so = COObject_GC_NEW(COSetObject, &COSet_Type);
    if (!so)
        return NULL;

    so->table == NULL;
    so->fill = 0;
    so->used = 0;
    INIT_NONZERO_SET_SLOTS(so);

    so->lookup = set_lookkey;

    if (iterable != NULL) {
        if (set_update_internal(so, iterable) == -1) {
            CO_DECREF(so);
            return NULL;
        }
    }

    COObject_GC_TRACK(so);
    return (COObject *)so;
}

COObject *
COSet_New(COObject *iterable)
{
    return make_new_set(&COSet_Type, iterable);
}

ssize_t
COSet_Size(COObject *this)
{
    if (!COSet_Check(this)) {
        COErr_BadInternalCall();
        return -1;
    }
    return COSet_GET_SIZE(this);
}

COTypeObject COSet_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "set",
    sizeof(COSetObject),
    0,
    COType_FLAG_GC,
    0,                          /* tp_new               */
    //(deallocfunc)set_dealloc,   /* tp_dealloc           */
    //(reprfunc)set_repr,         /* tp_repr              */
    0,                          /* tp_print             */
    0,                          /* tp_hash              */
    0,                          /* tp_compare           */
    //(traversefunc)set_traverse, /* tp_traverse          */
    //(inquiryfunc)set_clear,     /* tp_clear             */
    0,                          /* tp_call              */
    //(getiterfunc)set_iter,      /* tp_iter              */
    0,                          /* tp_iternext          */
    0,                          /* tp_int_interface     */
    0,                          /* tp_mapping_interface */
};
