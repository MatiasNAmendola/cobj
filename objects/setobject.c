#include "../cobj.h"

/* Object used as dummy key to fill deleted entries. */
static COObject *dummy = NULL;

/* Must be >= 1. */
#define PERTURB_SHIFT 5

#define INIT_NONZERO_SET_SLOTS(so) do {                         \
    (so)->table = (so)->smalltable;                             \
    (so)->mask = COSet_MINSIZE - 1;                             \
    } while(0)

#define EMPTY_TO_MINSIZE(so) do {                               \
    memset((so)->smalltable, 0, sizeof((so)->smalltable));      \
    (so)->used = (so)->fill = 0;                                \
    INIT_NONZERO_SET_SLOTS(so);                                 \
    } while(0)

static int set_next(COSetObject *so, ssize_t * pos_ptr,
                    struct setentry **entry_ptr);

static COObject *
set_repr(COSetObject *this)
{
    if (this->used == 0) {
        return COStr_FromString("set()");
    }
    COObject *s;
    s = COStr_FromString("set(");
    ssize_t pos = 0;
    struct setentry *entry;
    bool first = true;
    while (set_next(this, &pos, &entry)) {
        if (first) {
            first = false;
        } else {
            COStr_ConcatAndDel(&s, COStr_FromString(", "));
        }
        COStr_ConcatAndDel(&s, COObject_Repr(entry->key));
    }
    COStr_ConcatAndDel(&s, COStr_FromString(")"));
    return s;
}

static void
set_dealloc(COSetObject *this)
{
    COObject_GC_Free(this);
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

    for (perturb = hash;; perturb >>= PERTURB_SHIFT) {
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
    COErr_SetString(COException_UndefinedError, "TODO");
    return 0;
}

/**
 * Iterate over a set table. Use like so:
 *
 *  ssize_t  pos = 0;
 *  struct setentry *entry;
 *  while (set_next(set, &pos, &entry)) {
 *      // do stuff with pos & entry
 *  }
 */
static int
set_next(COSetObject *so, ssize_t * pos_ptr, struct setentry **entry_ptr)
{
    ssize_t i;
    ssize_t mask;
    struct setentry *table;

    table = so->table;
    mask = so->mask;
    i = *pos_ptr;

    while (i <= mask && (table[i].key == NULL || table[i].key == dummy))
        i++;

    *pos_ptr = i + 1;

    if (i > mask)
        return 0;

    *entry_ptr = &table[i];
    return 1;
}

static int
set_insert_key(COSetObject *so, COObject *key, long hash)
{
    struct setentry *entry;

    entry = so->lookup(so, key, hash);
    if (entry == NULL)
        return -1;
    if (entry->key == NULL) {
        // unused
        so->fill++;
        entry->key = key;
        entry->hash = hash;
        so->used++;
    } else if (entry->key == dummy) {
        // dummy
        entry->key = key;
        entry->hash = hash;
        so->used++;
        CO_DECREF(dummy);
    } else {
        // active
        CO_DECREF(key);
    }
    return 0;
}

static int
set_add_key(COSetObject *so, COObject *key)
{
    long hash;
    ssize_t n_used;

    /* at least one empty slot */
    assert(so->fill <= so->mask);
    n_used = so->used;

    CO_INCREF(key);
    hash = COObject_Hash(key);
    if (set_insert_key(so, key, hash) == -1) {
        CO_DECREF(key);
        return -1;
    }

    /*if (!(so->used > n_used && so->fill * 3 */
    return 0;
}

static int
set_update_internal(COSetObject *so, COObject *other)
{
    if (COSet_Check(other))
        return set_merge(so, other);

    COObject *iter;
    COObject *key;

    iter = COObject_GetIter(other);
    if (iter == NULL)
        return -1;

    while ((key = COObject_IterNext(iter)) != NULL) {
        if (set_add_key(so, key) == -1) {
            CO_DECREF(iter);
            CO_DECREF(key);
            return -1;
        }
        CO_DECREF(key);
    }

    CO_DECREF(iter);

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

    memset((so)->smalltable, 0, sizeof((so)->smalltable));
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

static COObject *
set_new(COTypeObject *type, COObject *args)
{
    COObject *x = NULL;
    if (!COObject_ParseArgs(args, &x, NULL))
        return NULL;
    return make_new_set(type, x);
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
    (newfunc)set_new,           /* tp_new               */
    0,                          /* tp_init */
    (deallocfunc)set_dealloc,   /* tp_dealloc           */
    (reprfunc)set_repr,         /* tp_repr              */
    0,                          /* tp_print             */
    0,                          /* tp_hash              */
    0,                          /* tp_compare           */
    0,                          //(traversefunc)set_traverse, /* tp_traverse          */
    0,                          //(inquiryfunc)set_clear,     /* tp_clear             */
    0,                          /* tp_call              */
    0,                          //(getiterfunc)set_iter,      /* tp_iter              */
    0,                          /* tp_iternext          */
    0,                          /* tp_arithmetic_interface     */
    0,                          /* tp_mapping_interface */
    0,                          /* tp_sequence_interface */
};
