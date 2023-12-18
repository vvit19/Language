#include "lang_tree.h"

Node* CreateOpNode (Node* left, Node* right, KeyWords op)
{
    Node* node = (Node*) calloc (1, sizeof (Node));
    assert (node);

    node->left     = left;
    node->right    = right;

    node->type     = OP_T;
    node->value.op = op;

    return node;
}

Node* CreateVarNode (Node* left, Node* right, const char* var)
{
    Node* node = (Node*) calloc (1, sizeof (Node));
    assert (node);

    node->left     = left;
    node->right    = right;

    node->type     = VAR_T;
    strcpy (node->value.var, var);

    return node;
}

Node* CreateNumNode (Node* left, Node* right, elem_t num)
{
    Node* node = (Node*) calloc (1, sizeof (Node));
    assert (node);

    node->left      = left;
    node->right     = right;

    node->type      = NUM_T;
    node->value.num = num;

    return node;
}

void TreeDtor (Node* node)
{
    if (!node) return;

    if (node->left)  TreeDtor (node->left);
    if (node->right) TreeDtor (node->right);

    free (node);
    node = nullptr;
}
