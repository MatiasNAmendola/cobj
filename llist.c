#include "co.h"

void
co_llist_init(co_llist *l, size_t size, llist_dtoc_func_t dtor)
{
    l->head = NULL;
    l->tail = NULL;
    l->count = 0;
    l->size = size;
    l->dtor = dtor;
}

void
co_llist_append_element(co_llist *l, void *element)
{
    co_llist_element *tmp = co_malloc(sizeof(co_llist_element) + l->size - 1);

    tmp->prev = l->tail;
    tmp->next = NULL;
    if (l->tail) {
        l->tail->next = tmp;
    } else {
        l->head = tmp;
    }
    l->tail = tmp;
    memcpy(tmp->data, element, l->size);

    l->count++;
}

void
co_llist_prepend_element(co_llist *l, void *element)
{
    co_llist_element *tmp = co_malloc(sizeof(co_llist_element) + l->size - 1);

    tmp->next = l->head;
    tmp->prev = NULL;
    if (l->head) {
        l->head->prev = tmp;
    } else {
        l->tail = tmp;
    }
    l->head = tmp;
    memcpy(tmp->data, element, l->size);

    l->count++;
}

void
co_llist_del_element(co_llist *l, void *element,
                     int (*compare) (void *element1, void *element2))
{
    co_llist_element *current = l->head;
    co_llist_element *next;

    while (current) {
        next = current->next;
        if (!compare(current->data, element)) {
            // delete it
            if (current->prev) {
                current->prev->next = current->next;
            } else {
                l->head = current->next;
            }
            if (current->next) {
                current->next->prev = current->prev;
            } else {
                l->tail = current->prev;
            }
            if (l->dtor) {
                l->dtor(current->data);
            }
            free(current);
            l->count--;
            break;
        }
        current = next;
    }
}

int
co_llist_search(co_llist *l, void *element,
                int (*compare) (void *element1, void *element2))
{
    co_llist_element *current = l->head;
    co_llist_element *next;

    while (current) {
        next = current->next;
        if (!compare(current->data, element)) {
            return 0;
        }
        current = next;
    }
    return 1;
}

int
co_llist_count(co_llist *l)
{
    return l->count;
}

void
co_llist_destory(co_llist *l)
{
    co_llist_element *current = l->head;
    co_llist_element *next;

    while (current) {
        next = current->next;
        if (l->dtor) {
            l->dtor(current->data);
        }
        free(current);
        current = next;
    }
}

void
co_llist_clean(co_llist *l)
{
    co_llist_destory(l);
    l->head = l->tail = NULL;
}

void *
co_llist_remove_tail(co_llist *l)
{
    co_llist_element *old_tail;
    void *data;

    if ((old_tail = l->tail)) {
        if (old_tail->prev) {
            old_tail->prev->next = NULL;
        } else {
            l->head = NULL;
        }
        l->tail = old_tail->prev;

        data = old_tail->data;
        if (l->dtor) {
            l->dtor(old_tail->data);
        }
        free(old_tail);

        l->count--;
        return data;
    }

    return NULL;
}

void
co_llist_copy(co_llist *dst, co_llist *src)
{
    co_llist_element *ptr;

    co_llist_init(dst, src->size, src->dtor);
    ptr = src->head;
    while (ptr) {
        co_llist_append_element(dst, ptr->data);
        ptr = ptr->next;
    }
}
