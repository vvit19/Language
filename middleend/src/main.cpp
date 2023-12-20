#include "middle.h"

int main (int argc, const char** argv)
{
    char* buffer = GetFileContent (TREE_TXT);
    Node* main_node = GetTree (buffer);
    free (buffer);

    Optimize (&main_node);
    ASTDump  (main_node);

    TreeDtor (main_node);
}
