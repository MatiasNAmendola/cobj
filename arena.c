#include "cobj.h"

#define DEFAULT_BLOCK_SIZE 8192
#define ALIGNMENT       8
#define ALIGNMENT_MASK  (ALIGNMENT - 1)
#define ROUNDUP(x)      (((x) + ALIGNMENT_MASK) & ~ALIGNMENT_MASK)

struct block {
    /* Total number of bytes owned by this block available to pass out.
     * Read-only after initialization. The first such byte starts at b_mem;
     */
    size_t b_size;

    /* Total number of bytes already passed out. The next byte available to pass
     * out starts at b_mem + b_offset.
     */
    size_t b_offset;

    /*
     * An arena maintains a singly-linked, NULL-terminated list of all blocks
     * owned by the arena. There are linked via the b_next member.
     */
    struct block *b_next;

    /* Pointer to the first allocatable byte owned by this block.
     */
    void *b_mem;
};

struct arena {
    /* Pointer to the first block allocated for the arena, never NULL.
     * It's used only to find the first block when the arena is beeing freed.
     */
    struct block *a_head;

    /* Pointer to the block currently used for allocation. It's b_next should be
     * NULL. If it is not-null after a call to block_alloc(), it means a new
     * block has been allocated and a_cur should be reset to point it.
     */
    struct block *a_cur;

    /* A list object containing references to all the COObject pointers. They
     * will be DECREFed when the arena is freed.
     */
    COObject *a_objects;
};

static struct block *
block_new(size_t size)
{
    struct block *b =
        (struct block *)COObject_Mem_MALLOC(sizeof(struct block) + size);
    if (!b)
        return NULL;
    b->b_size = size;
    b->b_mem = (void *)(b + 1);
    b->b_next = NULL;
    b->b_offset = ROUNDUP((uintptr_t) (b->b_mem)) - (uintptr_t) (b->b_mem);
    return b;
}

static void
block_free(struct block *b)
{
    while (b) {
        struct block *next = b->b_next;
        COObject_Mem_FREE(b);
        b = next;
    }
}

static void *
block_alloc(struct block *b, size_t size)
{
    void *p;
    size = ROUNDUP(size);
    if (b->b_offset + size > b->b_size) {
        /* If we need to allocated more memory than will fit in the default
         * block, allocate a one-off block that is exactly the right size. */
        struct block *new =
            block_new(size < DEFAULT_BLOCK_SIZE ? DEFAULT_BLOCK_SIZE : size);
        if (!new)
            return NULL;
        b->b_next = new;
        b = new;
    }

    assert(b->b_offset + size <= b->b_size);
    p = (void *)(((char *)b->b_mem) + b->b_offset);
    b->b_offset += size;
    return p;
}

struct arena *
arena_new(void)
{
    struct arena *arena =
        (struct arena *)COObject_Mem_MALLOC(sizeof(struct arena));
    if (!arena)
        return NULL;

    arena->a_head = block_new(DEFAULT_BLOCK_SIZE);
    if (!arena->a_head) {
        COObject_Mem_FREE(arena);
        return NULL;
    }
    arena->a_cur = arena->a_head;

    arena->a_objects = COList_New(0);
    if (!arena->a_objects) {
        block_free(arena->a_head);
        COObject_Mem_FREE(arena);
        return NULL;
    }

    return arena;
}

void *
arena_malloc(struct arena *arena, size_t size)
{
    void *p = block_alloc(arena->a_cur, size);
    if (!p)
        return NULL;

    /* Reset cur if we allocated a new block. */
    if (arena->a_cur->b_next) {
        arena->a_cur = arena->a_cur->b_next;
    }

    return p;
}

void
arena_free(struct arena *arena)
{
    block_free(arena->a_head);
    CO_DECREF(arena->a_objects);
    COObject_Mem_FREE(arena);
}

int
arena_addobject(struct arena *arena, COObject *o)
{
    // Don't increment ref count.
    int r = COList_Append(arena->a_objects, o);
    if (r >= 0) {
        CO_DECREF(o);
    }
    return 0;
}
