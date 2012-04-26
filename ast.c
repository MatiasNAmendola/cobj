#include "co.h"

/*
 * Create a node.
 */
Node *
node_new(Node_Type type, Node *nleft, Node *nright)
{
    Node *n;
    n = COMem_MALLOC(sizeof(*n));
    memset(n, 0, sizeof(*n));
    n->type = type;
    n->left = nleft;
    n->right = nright;
    return n;
}

/*
 * Append a node into list.
 */
NodeList *
nodelist_append(NodeList *l, Node *n)
{
    return nodelist_concat(l, nodelist(n, NULL));
}

/*
 * Concatenate two list.
 */
NodeList *
nodelist_concat(NodeList *a, NodeList *b)
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
nodelist(Node *n, ...)
{
    va_list params;

    va_start(params, n);

    NodeList *l;

    if (n) {
        l = COMem_MALLOC(sizeof(*l));
        memset(l, 0, sizeof(*l));
        l->n = n;
        l->next = NULL;
        l->end = l;
    }

    while (true) {
        n = va_arg(params, Node *);
        if (!n)
            break;
        l = nodelist_concat(l, nodelist(n, NULL));
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
        GIVE_NAME(NODE_BIN);    /* binary op node */
        GIVE_NAME(NODE_CMP);
        GIVE_NAME(NODE_UNARY);
        GIVE_NAME(NODE_IF);
        GIVE_NAME(NODE_WHILE);
        GIVE_NAME(NODE_FUNC);
        GIVE_NAME(NODE_FUNC_CALL);
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
nodelisttree(NodeList *l)
{
    indent_printf(0, "NodeList(%)\n", l);
    Node *n;
    for (; l; l = l->next) {
        n = l->n;
        node_print(n);
    }
}

int
nodelist_len(NodeList *l)
{
    int i = 0;
    while (l) {
        i++;
        l = l->next;
    }
    return i;
}

NodeList *
nodelist_changetype(NodeList *l, Node_Type t)
{
    while (l) {
        l->n->type = t;
        l = l->next;
    }
    return l;
}

void
node_free(Node *n)
{
#define TRY_NODE_FREE(a) \
    if (n->a) { \
        node_free(n->a); \
    }

#define TRY_NODELIST_FREE(l) \
    if (n->l) { \
        nodelist_free(n->l); \
    }

    if (!n) {
        return;
    }

    /*if (n->left) {*/
    /*node_free(n->left);*/
    /*}*/
    /*if (n->right) {*/
    /*node_free(n->right);*/
    /*}*/

    /*TRY_NODELIST_FREE(list);*/

    /*TRY_NODE_FREE(ntest);*/
    /*TRY_NODELIST_FREE(nbody);*/
    /*TRY_NODELIST_FREE(nelse);*/

    /*TRY_NODE_FREE(nfuncname);*/
    /*TRY_NODELIST_FREE(nfuncargs);*/
    /*TRY_NODELIST_FREE(nfuncbody);*/

    /*TRY_NODELIST_FREE(ntrybody);*/
    /*TRY_NODELIST_FREE(ncatches);*/
    /*TRY_NODELIST_FREE(norelse);*/
    /*TRY_NODELIST_FREE(nfinally);*/

    /*TRY_NODELIST_FREE(ncatchname);*/
    /*TRY_NODELIST_FREE(ncatchbody);*/

    COMem_FREE(n);
}

void
nodelist_free(NodeList *l)
{
    NodeList *tmp;
    while (l) {
        tmp = l;
        node_free(l->n);
        l = l->next;
        COMem_FREE(tmp);
    }
}
