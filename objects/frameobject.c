#include "../co.h"

static COObject *
stack_repr(COFrameObject *this)
{
    return COStr_FromString("<stack>");
}

COTypeObject COFrame_Type = {
    COObject_HEAD_INIT(&COType_Type),
    "stack",
    sizeof(COFrameObject),
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

#define VM_STACK_PAGE_SIZE (64*1024)
#define VM_STACK_GROW_IF_NEEDED(size)                                  \
    if (size > ((COFrameObject *)this)->vm_stack->end                  \
            - ((COFrameObject *)this)->vm_stack->top) {                \
            vm_stack_extend(this, size);                               \
    }

static struct vm_stack *
vm_stack_new_page(size_t size)
{
    struct vm_stack *page =
        (struct vm_stack *)COMem_MALLOC(sizeof(struct vm_stack) +
                                        sizeof(void*) * (size - 1));
    page->top = page->elements;
    page->end = page->elements + size;
    page->prev = NULL;
    return page;
}

static void
vm_stack_extend(COObject *this, size_t size)
{
    struct vm_stack *p =
        vm_stack_new_page(size >
                          VM_STACK_PAGE_SIZE ? size : VM_STACK_PAGE_SIZE);
    p->prev = ((COFrameObject *)this)->vm_stack;
    ((COFrameObject *)this)->vm_stack = p;
}

COObject *
COFrame_New(void)
{
    COFrameObject *f = COObject_New(COFrameObject, &COFrame_Type);

    f->vm_stack = vm_stack_new_page(VM_STACK_PAGE_SIZE);
    return (COObject *)f;
}

COObject *
COFrame_Alloc(COObject *this, size_t size)
{
    void *ret;

    // ceil(size / sizeof(void*))
    size = (size + (sizeof(void *) - 1)) / sizeof(void *);

    VM_STACK_GROW_IF_NEEDED(size);

    ret = ((COFrameObject *)this)->vm_stack->top;
    ((COFrameObject *)this)->vm_stack->top += size;
    return ret;
}

COObject *
COFrame_Pop(COObject *this)
{
    void *e = *(--((COFrameObject *)this)->vm_stack->top);

    if (((COFrameObject *)this)->vm_stack->top ==
        ((COFrameObject *)this)->vm_stack->elements) {
        struct vm_stack *p = ((COFrameObject *)this)->vm_stack;

        ((COFrameObject *)this)->vm_stack = p->prev;
        assert(p->prev);
        COMem_FREE(p);
    }

    return e;
}

void
COFrame_Free(COObject *this, COObject *co)
{
    assert((void **)co <= ((COFrameObject *)this)->vm_stack->top);
    assert((void **)co >= ((COFrameObject *)this)->vm_stack->elements);
    if (((COFrameObject *)this)->vm_stack->elements == (void **)co) {
        struct vm_stack *p = ((COFrameObject *)this)->vm_stack;
        ((COFrameObject *)this)->vm_stack = p->prev;
        COMem_FREE(p);
    } else {
        // only mark it free
        ((COFrameObject *)this)->vm_stack->top = (void **)co;
    }
}

void
COFrame_Push(COObject *this, COObject *co)
{
    VM_STACK_GROW_IF_NEEDED(1);
    *(((COFrameObject *)this)->vm_stack->top++) = co;
}

void
COFrame_Destory(COObject *this)
{
    struct vm_stack *stack = ((COFrameObject *)this)->vm_stack;

    while (stack != NULL) {
        struct vm_stack *p = stack->prev;

        COMem_FREE(stack);
        stack = p;
    }
}
