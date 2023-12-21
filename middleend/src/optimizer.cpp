#include "middle.h"

static bool   OptimizeConstants (Node* node, bool* has_optimized);
static int    OptimizeNeutrals  (Node** parent, Node* node, bool* has_optimized);
static elem_t      Eval         (Node* node, elem_t var_value);

void Optimize (Node** node)
{
    assert (node);

    bool has_optimized = false;

    while (true)
    {
        OptimizeConstants (*node, &has_optimized);
        OptimizeNeutrals  (node, *node, &has_optimized);

        if (has_optimized == false) return;
        has_optimized = false;
    }
}

bool OptimizeConstants (Node* node, bool* has_optimized)
{
    assert (node);

    if (node->type == NUM_T) return true;
    if (node->type == VAR_T) return false;

    bool optimize_left  = false;
    bool optimize_right = false;

    if (node->left)  optimize_left  = OptimizeConstants (node->left, has_optimized);
    if (node->right) optimize_right = OptimizeConstants (node->right, has_optimized);

    if (optimize_left && optimize_right)
    {
        node->value.num = Eval (node, 0.0);
        node->type = NUM_T;

        TreeDtor (node->left);
        TreeDtor (node->right);

        node->left  = nullptr;
        node->right = nullptr;

        *has_optimized = true;
        return true;
    }

    return false;
}

int OptimizeNeutrals (Node** parent, Node* node, bool* has_optimized)
{
    assert (parent);
    assert (node);

    if (node->type == NUM_T && IsInt (node->value.num)) return (int) node->value.num;

    if (node->type == VAR_T) return -1;

    int optimize_left  = -1;
    int optimize_right = -1;

    if (node->left)  optimize_left  = OptimizeNeutrals (&node, node->left,  has_optimized);
    if (node->right) optimize_right = OptimizeNeutrals (&node, node->right, has_optimized);

    if ( (optimize_left  == 0 && (CUR_OP == MULT || CUR_OP == DIV)) ||
         (optimize_right == 0 && (CUR_OP == MULT)) )
    {
        node->value.num = 0;
        node->type = NUM_T;

        TreeDtor (node->left);
        TreeDtor (node->right);

        node->left  = nullptr;
        node->right = nullptr;

        *has_optimized = true;
    }
    else if ((optimize_left == 0 && CUR_OP == ADD) || (optimize_left == 1 && CUR_OP == MULT))
    {
        if (node == *parent)
        {
            *parent = node->right;
        }
        else
        {
            if (node == (*parent)->left) (*parent)->left = node->right;
            else (*parent)->right = node->right;
        }

        TreeDtor (node->left);
        free (node);
        node = nullptr;

        *has_optimized = true;
    }
    else if ((optimize_right == 0 && CUR_OP == ADD)  ||
             (optimize_right == 1 && CUR_OP == MULT) ||
             (optimize_right == 0 && CUR_OP == SUB)  ||
             (optimize_right == 1 && CUR_OP == DIV))
    {
        if (node == *parent) *parent = node->left;
        else
        {
            if (node == (*parent)->left) (*parent)->left = node->left;
            else (*parent)->right = node->left;
        }

        TreeDtor (node->right);
        free (node);
        node = nullptr;

        *has_optimized = true;
    }

    return -1;
}

static elem_t Eval (Node* node, elem_t var_value)
{
    if (!node) return NAN;

    if (node->type == NUM_T) return node->value.num;
    if (node->type == VAR_T) return var_value;

    elem_t left_value  = Eval (node->left,  var_value);
    elem_t right_value = Eval (node->right, var_value);

    switch (node->value.op)
    {
    case ADD:  return left_value + right_value;
    case SUB:  return left_value - right_value;
    case MULT: return left_value * right_value;
    case DIV:  return left_value / right_value;
    case SIN:  return sin  (left_value);
    case COS:  return cos  (left_value);
    case SQRT: return sqrt (left_value);

    default: NO_PROPER_CASE_FOUND;
    }

    return NAN;
}
