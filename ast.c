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
