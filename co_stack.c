#include "co_stack.h"

bool
co_stack_init(co_stack * stack)
{
    stack->top = 0;
    stack->elements = (void **)xmalloc(sizeof(void *) * STACK_BLOCK_SIZE);
    if (!stack->elements) {
        return false;
    } else {
        stack->max = STACK_BLOCK_SIZE;
        return true;
    }
}

bool
co_stack_push(co_stack * stack, const void *element, int size)
{
    if (stack->top >= stack->max) {     /* we need to allocate more memory */
        stack->max += STACK_BLOCK_SIZE;
        stack->elements = (void **)xrealloc(stack->elements, sizeof(void *) * stack->max);
        if (!stack->elements) {
            return false;
        }
    }

    stack->elements[stack->top] = (void *)xmalloc(size);
    memcpy(stack->elements[stack->top], element, size);
    return stack->top++;
}

bool
co_stack_top(const co_stack * stack, void **element)
{
    if (stack->top > 0) {
        *element = stack->elements[stack->top - 1];
        return true;
    } else {
        *element = NULL;
        return false;
    }
}

bool
co_stack_pop(co_stack * stack)
{
    if (stack->top > 0) {
        free(stack->elements[--stack->top]);
    }
    return true;
}

bool
co_stack_is_empty(const co_stack * stack)
{
    if (stack->top == 0) {
        return true;
    } else {
        return false;
    }
}

bool
co_stack_destory(co_stack * stack)
{
    if (stack->elements) {
        while (--stack->top >= 0) {
            free(stack->elements[stack->top]);
        }
    }

    return true;
}

void **
co_stack_base(const co_stack * stack)
{
    return stack->elements;
}

int
co_stack_count(const co_stack * stack)
{
    return stack->top;
}

void
co_stack_apply(co_stack * stack, int type, int (*apply_function) (void *element))
{
    int i;

    switch (type) {
    case CO_STACK_APPLY_TYPE_TOPDOWN:
        for (i = stack->top - 1; i >= 0; i--) {
            if (apply_function(stack->elements[i])) {
                break;
            }
        }
        break;
    case CO_STACK_APPLY_TYPE_BOTTOMUP:
        for (i = 0; i < stack->top; i++) {
            if (apply_function(stack->elements[i])) {
                break;
            }
        }
        break;
    }
}
