#include "co.h"

/*
 * Create a node.
 */
Node *
node_new(Node_Type type, Node *nleft, Node *nright)
{
    Node *n;
    n = COMem_MALLOC(sizeof(*n));
    n->type = type;
    n->left = nleft;
    n->right = nright;
    return n;
}

NodeList *
node_concat(NodeList *a, NodeList *b)
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
NodeList *
node_list(Node *n, ...)
{
    va_list params;

    va_start(params, n);

    NodeList *l;

    if (n) {
        l = COMem_MALLOC(sizeof(*l));
        l->n = n;
        l->next = NULL;
        l->end = l;
    }

    while (true) {
        n = va_arg(params, Node *);
        if (!n)
            break;
        l = node_concat(l, node_list(n, NULL));
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
        GIVE_NAME(NODE_IF);
        GIVE_NAME(NODE_WHILE);
        GIVE_NAME(NODE_BIN);
        GIVE_NAME(NODE_RETURN);
        GIVE_NAME(NODE_CONST);
        GIVE_NAME(NODE_NAME);
        GIVE_NAME(NODE_ASSIGN);
        GIVE_NAME(NODE_PRINT);
    }
    error("unknow type: %d\n", type);
    return NULL;
}

/*
 * List node tree.
 */
void
node_listtree(NodeList *l)
{
    printf("List Tree: %p\n", l);
    Node *n;
    int i = 0;
    for (; l; l = l->next) {
        n = l->n;
        printf("  %d. %p (type: %d)\n", i, n, n->type);
        i++;
    }
}
