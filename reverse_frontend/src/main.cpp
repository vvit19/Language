#include "retranslator.h"

int main ()
{
    char* buffer = GetFileContent (TREE_TXT);
    Node* main_node = GetTree (buffer);
    free (buffer);

    GetLangCode (main_node);

    TreeDtor (main_node);
}
