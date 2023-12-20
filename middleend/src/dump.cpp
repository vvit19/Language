#include "middle.h"

#define _print(...) fprintf (file, __VA_ARGS__);

const char* const OUTPUT_AST  = "tree.txt";

static void DumpNode (Node* node, FILE* file);

void ASTDump (Node* main_node)
{
    assert (main_node);

    FILE* file = fopen (OUTPUT_AST, "w");
    assert (file);

    DumpNode (main_node, file);

    _print ("\n");
    fclose (file);
}

void DumpNode (Node* node, FILE* file)
{
    assert (file);

    if (!node)
    {
        _print ("{ NIL }");
        return;
    }

    _print ("{");

    switch (node->type)
    {
        case OP_T:
            _print (" %s ", OperationsArray[node->value.op]);
            break;

        case NUM_T:
            _print (" %lg ", node->value.num);
            break;

        case FUNC_NAME:
        case VAR_T:
            _print (" %s ", node->value.var);
            break;

        default:
            NO_PROPER_CASE_FOUND;
            break;
    }

    DumpNode (node->left,  file);
    DumpNode (node->right, file);

    _print ("}");
}
