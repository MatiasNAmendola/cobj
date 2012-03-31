#include "co.h"

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
