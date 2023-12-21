#include "retranslator.h"

static void PrintNode (Node* node, FILE* file, int level, KeyWords prev_op);
static void PrintTabs (FILE* file, int level);

void GetLangCode (Node* main_node)
{
    assert (main_node);

    FILE* file = fopen (OUTPUT, "w");
    assert (file);

    PrintNode (main_node, file, 0, FUNC);
}

static void PrintNode (Node* node, FILE* file, int level, KeyWords prev_op)
{
    assert (file);
    if (!node) return;

    if (node->type == NUM_T)
    {
        fprintf (file, "%lg", node->value.num);
    }

    if (node->type == VAR_T)
    {
        fprintf (file, "%s", node->value.var);
    }

    if (node->type == FUNC_NAME)
    {
        fprintf (file, "%s ", node->value.var);
        if (prev_op != CALL) fprintf (file, "(");

        PrintNode (node->left,  file, level, prev_op);
        PrintNode (node->right, file, level, prev_op);

        if (!node->right && prev_op != CALL) fprintf (file, ")");
    }

    if (node->type == OP_T)
    {
        if (node->value.op == FUNC)
        {
            fprintf (file, "func ");
            PrintNode (node->left, file, level, prev_op);
            fprintf (file, "\n{\n");
            PrintNode (node->right, file, level + 1, prev_op);
            fprintf (file, "}");

            return;
        }

        if (node->value.op == PARAM)
        {
            PrintNode (node->left, file, level, prev_op);

            if (node->right) fprintf (file, ", ");

            PrintNode (node->right, file, level, prev_op);

            return;
        }

        if (node->value.op == VAR)
        {
            PrintTabs (file, level);

            fprintf (file, "var %s = ", node->left->value.var);

            PrintNode (node->right->left, file, level, prev_op);
            PrintNode (node->right->right, file, level, prev_op);
            return;
        }

        if (node->value.op == CALL)
        {
            fprintf (file, "%s ", LanguageSyntax[node->value.op]);
            PrintNode (node->left, file, level, CALL);
            fprintf (file, "(");
            PrintNode (node->right, file, level, CALL);
            fprintf (file, ")");

            return;
        }

        if (node->value.op == IN || node->value.op == OUT)
        {
            PrintTabs (file, level);

            fprintf (file, "%s (", LanguageSyntax[node->value.op]);
            PrintNode (node->left, file, level, prev_op);
            fprintf (file, ")");
            PrintNode (node->right, file, level, prev_op);

            return;
        }

        if (node->value.op == RET)
        {
            PrintTabs (file, level);
            fprintf (file, "%s ", LanguageSyntax[node->value.op]);
            PrintNode (node->left, file, level, prev_op);

            return;
        }

        if (node->value.op == IF || node->value.op == WHILE)
        {
            PrintTabs (file, level);

            fprintf (file, "%s (", LanguageSyntax[node->value.op]);
            PrintNode (node->left, file, level, prev_op);
            fprintf (file, ")\n");
            PrintTabs (file, level);
            fprintf (file, "{\n");
            PrintNode (node->right, file, level + 1, prev_op);
            PrintTabs (file, level);
            fprintf (file, "}");

            return;
        }

        if (node->value.op == EQ) PrintTabs (file, level);

        PrintNode (node->left, file, level, prev_op);

        if (node->value.op == NEXT_OP)
        {
            fprintf (file, "%s\n", LanguageSyntax[node->value.op]);
        }
        else fprintf (file, " %s ", LanguageSyntax[node->value.op]);

        PrintNode (node->right, file, level, prev_op);
    }
}

static void PrintTabs (FILE* file, int level)
{
    for (int i = 0; i < level; i++) fputc ('\t', file);
}
