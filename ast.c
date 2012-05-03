#include "co.h"

/*
 * Create a node.
 */
Node *
node_new(struct arena *arena, Node_Type type, Node *nleft, Node *nright)
{
    Node *n;
    n = arena_malloc(arena, sizeof(*n));
    memset(n, 0, sizeof(*n));
    n->type = type;
    n->left = nleft;
    n->right = nright;
    return n;
}

/*
 * Append a node into list.
 */
Node *
nodelist_append(struct arena *arena, Node *l, Node *n)
{
    return nodelist_concat(l, nodelist(arena, n, NULL));
}

/*
 * Concatenate two list.
 */
Node *
nodelist_concat(Node *a, Node *b)
{
    a->end->next = b;
    a->end = b->end;
    b->end = NULL;
    return a;
}

/* 
 * Construct a node list from list of nodes, last arg should be NULL to end
 * argument list.
 */
Node *
nodelist(struct arena *arena, Node *n, ...)
{
    va_list params;

    va_start(params, n);

    Node *l;

    if (n) {
        l = node_new(arena, NODE_BLOCK, NULL, NULL);
        l->n = n;
        l->next = NULL;
        l->end = l;
    }

    while (true) {
        n = va_arg(params, Node *);
        if (!n)
            break;
        l = nodelist_concat(l, nodelist(arena, n, NULL));
    }

    va_end(params);
    return l;
}

const char *
node_type(Node_Type type)
{
#define GIVE_NAME(type) \
    case (type):        \
        return #type

    switch (type) {
        GIVE_NAME(NODE_BLOCK);
        GIVE_NAME(NODE_BIN);    /* binary op node */
        GIVE_NAME(NODE_CMP);
        GIVE_NAME(NODE_UNARY);
        GIVE_NAME(NODE_IF);
        GIVE_NAME(NODE_WHILE);
        GIVE_NAME(NODE_FUNC);
        GIVE_NAME(NODE_FUNC_CALL);
        GIVE_NAME(NODE_FUNC_CALL_STMT);
        GIVE_NAME(NODE_RETURN);
        GIVE_NAME(NODE_CONST);
        GIVE_NAME(NODE_NAME);
        GIVE_NAME(NODE_ASSIGN);
        GIVE_NAME(NODE_PRINT);
        GIVE_NAME(NODE_LIST);
        GIVE_NAME(NODE_DICT_BUILD);
        GIVE_NAME(NODE_DICT_ADD);
        GIVE_NAME(NODE_TRY);
        GIVE_NAME(NODE_CATCH);
        GIVE_NAME(NODE_BREAK);
        GIVE_NAME(NODE_CONTINUE);
        GIVE_NAME(NODE_THROW);
    }
    error("unknown type: %d\n", type);
    return NULL;
}

static void
indent_printf(int level, const char *fmt, ...)
{
    va_list params;
    va_start(params, fmt);

    for (int i = 0; i < level; i++)
        printf("----");
    vprintf(fmt, params);

    va_end(params);
}

void
node_print(Node *n)
{
    static int level = 1;
    indent_printf(level, "Node(%p, %s)\n", n, node_type(n->type));
    level++;
    /*if (n->left) { */
    /*indent_printf(level, "->left\n", n, node_type(n->type)); */
    /*node_print(n->left); */
    /*} */
    /*if (n->right) { */
    /*indent_printf(level, "->right\n", n, node_type(n->type)); */
    /*node_print(n->right); */
    /*} */
    /*if (n->list) { */
    /*indent_printf(level, "->list\n", n, node_type(n->type)); */
    /*nodelisttree(n->list); */
    /*} */
    level--;
}

/*
 * List node tree.
 */
void
nodelisttree(Node *l)
{
    indent_printf(0, "Node(%)\n", l);
    Node *n;
    for (; l; l = l->next) {
        n = l->n;
        node_print(n);
    }
}

int
nodelist_len(Node *l)
{
    int i = 0;
    while (l) {
        i++;
        l = l->next;
    }
    return i;
}

Node *
nodelist_changetype(Node *l, Node_Type t)
{
    while (l) {
        l->n->type = t;
        l = l->next;
    }
    return l;
}
