#include "cobj.h"

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
 * Construct a node list from list of nodes, last arg should be NULL to end
 * argument list.
 */
Node *
node_list(struct arena *arena, Node *n, ...)
{
    va_list params;

    va_start(params, n);

    Node *l = NULL;

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
        l = node_listconcat(l, node_list(arena, n, NULL));
    }

    va_end(params);
    return l;
}

/*
 * Append a node into list.
 */
Node *
node_listappend(struct arena *arena, Node *l, Node *n)
{
    return node_listconcat(l, node_list(arena, n, NULL));
}

/*
 * Concatenate two list.
 */
Node *
node_listconcat(Node *a, Node *b)
{
    a->nd_end->nd_next = b;
    a->nd_end = b->nd_end;
    b->nd_end = NULL;
    return a;
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
        GIVE_NAME(NODE_STORE_DOTSUBSCRIPT);
        GIVE_NAME(NODE_LOAD_SUBSCRIPT);
        GIVE_NAME(NODE_LOAD_DOTSUBSCRIPT);
        GIVE_NAME(NODE_CMP);
        GIVE_NAME(NODE_UNARY);
        GIVE_NAME(NODE_IF);
        GIVE_NAME(NODE_WHILE);
        GIVE_NAME(NODE_FOR);
        GIVE_NAME(NODE_FUNC);
        GIVE_NAME(NODE_FUNC_CALL);
        GIVE_NAME(NODE_FUNC_CALL_STMT);
        GIVE_NAME(NODE_RETURN);
        GIVE_NAME(NODE_CONST);
        GIVE_NAME(NODE_NAME);
        GIVE_NAME(NODE_ASSIGN);
        GIVE_NAME(NODE_ASSIGN_LOCAL);
        GIVE_NAME(NODE_TUPLE);
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

int
node_listlen(Node *l)
{
    int i = 0;
    while (l) {
        i++;
        l = l->nd_next;
    }
    return i;
}
