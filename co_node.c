/*
 * Parse tree node implementation
 */
#include "co.h"
#include "co_node.h"

node *
node_new(int type)
{
    node *n = (node *) xmalloc(sizeof(node));

    n->type = type;
    n->str = NULL;
    n->line = 0;
    n->col = 0;
    n->nchildren = 0;
    n->child = NULL;
    return n;
}

static int
fancy_roundup(int n)
{
    /* Round up to the closest power of 2 which is >= n. */
    int result = 256;

    assert(n > 128);
    while (result < n) {
        result <<= 1;
        if (result <= 0)
            return -1;
    }
    return result;
}

#define XROUNDUP(n) ((n) <= 1 ? (n) :       \
        (n) <= 128 ? (((n) + 3) & -3) :     \
        fancy_roundup(n))

bool
node_addchild(register node * n1, int type, char *str, int line, int col)
{
    const int nch = n1->nchildren;

    int current_capacity;

    int required_capacity;

    node *n;

    if (nch == INT_MAX || nch < 0)
        return false;

    current_capacity = XROUNDUP(nch);
    required_capacity = XROUNDUP(nch + 1);
    if (current_capacity < 0 || required_capacity < 0)
        return false;

    if (current_capacity < required_capacity) {
        if (required_capacity > INT_MAX / sizeof(node)) {
            return 0;
        }
        n1->child = (node *) xrealloc(n1->child, required_capacity * sizeof(node));
    }

    n = &n1->child[n1->nchildren++];
    n->type = type;
    n->str = str;
    n->line = line;
    n->col = col;
    n->nchildren = 0;
    n->child = NULL;
    return 0;
}

static void
freechildren(node * n)
{
    int i;

    for (i = NCH(n); --i >= 0;)
        freechildren(CHILD(n, i));
    if (n->child != NULL)
        free(n->child);
    if (STR(n) != NULL)
        free(STR(n));
}

void
node_free(node * n)
{
    if (n != NULL) {
        freechildren(n);
        free(n);
    }
}

/* Forward */
static void listnode(FILE * fp, node * n);

static int level, atbol;

void
listtree(node * n)
{
    level = 0;
    atbol = 1;
    listnode(stdout, n);
}

static void
listnode(FILE * fp, node * n)
{
    if (n == NULL)
        return;
    if (TYPE(n) < 256) {        // is noterminal
        int i;

        for (i = 0; i < NCH(n); i++)
            listnode(fp, CHILD(n, i));
    } else if (TYPE(n) > 256) { // is terminal
        switch (TYPE(n)) {
            default:
                if (atbol) {
                    int i;

                    for (i = 0; i < level; i++)
                        fprintf(fp, "\t");
                    atbol = 0;
                }
                fprintf(fp, "%s ", STR(n));
                break;
        }
    } else {
        fprintf(fp, "? ");
    }
}
