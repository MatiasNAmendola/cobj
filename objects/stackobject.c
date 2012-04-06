#include "../co.h"

static COObject *
stack_repr(COStackObject *this)
{
    return COStr_FromString("<stack>");
}

COTypeObject COStack_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "stack",
    sizeof(COStackObject),
    0,
    (reprfunc)stack_repr,       /* tp_repr */
    0,                          /* tp_getattr */
    0,                          /* tp_setattr */
    0,                          /* tp_hash */
    0,                          /* tp_compare */
    0,                          /* tp_int_interface */
};

struct vm_stack {
    void **top;
    void **end;
    struct vm_stack *prev;
    void *elements[1];
};

#define VM_STACK_PAGE_SIZE (64 * 1024)
#define VM_STACK_GROW_IF_NEEDED(size)                \
    if (size > ((COStackObject *)this)->vm_stack->end                  \
            - ((COStackObject *)this)->vm_stack->top) {                \
            vm_stack_extend(this, size);                   \
    }

static struct vm_stack *
vm_stack_new_page(size_t size)
{
    struct vm_stack *page =
        (struct vm_stack *)COMem_MALLOC(sizeof(struct vm_stack) +
                                        sizeof(page->elements[0]) * (size - 1));
    page->top = page->elements;
    page->end = page->elements + size;
    page->prev = NULL;
    return page;
}

static void
vm_stack_extend(COObject *this, size_t size)
{
    struct vm_stack *p =
        vm_stack_new_page(size >=
                          VM_STACK_PAGE_SIZE ? size : VM_STACK_PAGE_SIZE);
    p->prev = ((COStackObject *)this)->vm_stack;
    ((COStackObject *)this)->vm_stack = p;
}

COObject *
COStack_New(void)
{
    COStackObject *f = COObject_New(COStackObject, &COStack_Type);

    f->vm_stack = vm_stack_new_page(VM_STACK_PAGE_SIZE);
    return (COObject *)f;
}

COObject *
COStack_Alloc(COObject *this, size_t size)
{
    void *ret;

    // ceil(size / sizeof(void*))
    size = (size + (sizeof(void *) - 1)) / sizeof(void *);

    VM_STACK_GROW_IF_NEEDED(size);

    ret = (void *)((COStackObject *)this)->vm_stack->top;
    ((COStackObject *)this)->vm_stack->top += size;
    return ret;
}

COObject *
COStack_Pop(COObject *this)
{
    void *e = *(--((COStackObject *)this)->vm_stack->top);

    if (((COStackObject *)this)->vm_stack->top ==
        ((COStackObject *)this)->vm_stack->elements) {
        struct vm_stack *p = ((COStackObject *)this)->vm_stack;

        ((COStackObject *)this)->vm_stack = p->prev;
        free(p);
    }

    return e;
}

void
COStack_Free(COObject *this, COObject *co)
{
    if ((void **)((COStackObject *)this)->vm_stack == (void **)co) {
        // free if it's current stack frame
        struct vm_stack *p = ((COStackObject *)this)->vm_stack;
        ((COStackObject *)this)->vm_stack = p->prev;
        free(p);
    } else {
        // else only mark it free
        ((COStackObject *)this)->vm_stack->top = (void **)co;
    }
}

void
COStack_Push(COObject *this, COObject *co)
{
    VM_STACK_GROW_IF_NEEDED(1);
    *(((COStackObject *)this)->vm_stack->top++) = co;
}

void
COStack_Destory(COObject *this)
{
    struct vm_stack *stack = ((COStackObject *)this)->vm_stack;

    while (stack != NULL) {
        struct vm_stack *p = stack->prev;

        free(stack);
        stack = p;
    }
}
