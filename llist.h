#ifndef LLIST_H
#define LLIST_H
/**
 *
 * Linked list.
 */

typedef struct _co_llist_element co_llist_element;

typedef struct _co_llist co_llist;

struct _co_llist_element {
    struct _co_llist_element *next;
    struct _co_llist_element *prev;
    char data[1];               /* Needs to always be last in the struct */
};

typedef void (*llist_dtoc_func_t) (void *);

struct _co_llist {
    co_llist_element *head;
    co_llist_element *tail;
    size_t count;
    size_t size;                /* element's data size, a given llist's every elements have same size */
    llist_dtoc_func_t dtor;     /* element's destructor */
    co_llist_element *traverse_ptr;
};

extern void co_llist_init(co_llist *l, size_t size, llist_dtoc_func_t dtor);
extern void co_llist_append_element(co_llist *l, void *element);
extern void co_llist_prepend_element(co_llist *l, void *element);
extern void co_llist_del_element(co_llist *l, void *element,
                                 bool(*compare) (void *element1, void *element2));
extern void co_llist_destory(co_llist *l);
extern void co_llist_clean(co_llist *l);
extern void *co_llist_remove_tail(co_llist *l);
extern void co_llist_copy(co_llist *dst, co_llist *src);

#endif
