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
    n->nd_left = nleft;
    n->nd_right = nright;
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
    a->nd_end->nd_next = b;
    a->nd_end = b->nd_end;
    b->nd_end = NULL;
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
        l = node_new(arena, NODE_TREE, NULL, NULL);
        l->nd_node = n;
        l->nd_next = NULL;
        l->nd_end = l;
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
        GIVE_NAME(NODE_TREE);
        GIVE_NAME(NODE_BIN);
        GIVE_NAME(NODE_STORE_SUBSCRIPT);
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
    for (; l; l = l->nd_next) {
        n = l->nd_node;
        node_print(n);
    }
}

int
nodelist_len(Node *l)
{
    int i = 0;
    while (l) {
        i++;
        l = l->nd_next;
    }
    return i;
}
