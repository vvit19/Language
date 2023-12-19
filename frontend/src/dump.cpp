#include "dump.h"
#include "dsl.h"

static void GraphNodeDump   (FILE* file, Node* node);
static void DrawConnections (FILE* file, Node* node);

#define _print(...) fprintf (file, __VA_ARGS__)

const int NIL_LENGTH = 3;

void GraphDump (Node* node)
{
    assert (node);

    FILE* file = fopen (OUTPUT_DOT, "w");

    _print (R"(
            digraph g {
            rankdir   =  TB;
            graph[ranksep = 1.3, nodesep = 0.5, style = "rounded, filled"]
            )");

    GraphNodeDump (file, node);

    DrawConnections (file, node);

    _print ("}\n");

    fclose (file);

    char dot_command[150] = "";
    sprintf (dot_command, "dot -Tpng %s -o %s", OUTPUT_DOT, OUTPUT_PNG);
    system (dot_command);
}

static void GraphNodeDump (FILE* file, Node* node)
{
    if (node == nullptr) return;

    if (node->type == VAR_T || node->type == FUNC_NAME)
    {
        _print ("Node%p[shape=rectangle, width=0.2, style=\"filled\","
                "fillcolor=\"lightblue\", label=\"%s\"] \n \n",
                node, node->value.var);
    }
    else if (node->type == OP_T)
    {
        switch (node->value.op)
        {
        case SIN:
            _print ("Node%p[shape=rectangle, width=0.2, style=\"filled\","
            "fillcolor=\"yellow\", label=\"sin\"] \n \n", node);
            break;

        case COS:
            _print ("Node%p[shape=rectangle, width=0.2, style=\"filled\","
            "fillcolor=\"yellow\", label=\"cos\"] \n \n", node);
            break;

        default:
            _print ("Node%p[shape=rectangle, width=0.2, style=\"filled\","
                    "fillcolor=\"yellow\", label=\"%s\"] \n \n",
                    node, LanguageSyntax[node->value.op]);
        }
    }
    else if (node->type == NUM_T)
    {
        _print ("Node%p[shape=rectangle, width=0.2, style=\"filled\","
                "fillcolor=\"orange\", label=\"%lf\"] \n \n",
                node, node->value.num);
    }

    GraphNodeDump (file, node->left);
    GraphNodeDump (file, node->right);
}

static void DrawConnections (FILE* file, Node* node)
{

    if (node->left && node->left->type != NO_TYPE)
    {
        _print ("Node%p->Node%p\n", node, node->left);
        DrawConnections (file, node->left);
    }

    if (node->right && node->right->type != NO_TYPE)
    {
        _print ("Node%p->Node%p\n", node, node->right);
        DrawConnections (file, node->right);
    }
}

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
