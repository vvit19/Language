#include "backend.h"

int main (int argc, const char** argv)
{
    if (argc != 2)
    {
        fprintf (stderr, RED_COLOR "Неправильный запуск программы (ввод аргументов командной строки)\n" RESET_COLOR);
        return -1;
    }

    char* buffer = GetFileContent (TREE_TXT);
    Node* main_node = GetTree (buffer);
    free (buffer);

    GetAsmCode (argv[1], main_node);

    TreeDtor (main_node);
}
