#ifndef CO_NODE_H
#define CO_NODE_H

typedef struct _node {
    short type;
    char *str;
    int line;
    int col;
    int nchildren;
    struct _node *child;
} node;

extern node *root;

extern node *node_new(int type);

extern bool node_addchild(node * n, int type, char *str, int line, int col);

extern void node_free(node * n);

/* Node access functins */
#define NCH(n)          ((n)->nchildren)
#define CHILD(n, i)     (&(n)->child[i])
#define RCHILD(n, i)    (CHILD(n, NCH(n) + i))
#define TYPE(n)         ((n)->type)
#define STR(n)          ((n)->str)

/* Assert that the type of a node is what we expect */
#define REQ(n, type)    assert(TYPE(n) == (type))

extern void listtree(node * n);

#endif
