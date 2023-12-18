#include "backend.h"

int main (int argc, const char** argv)
{
    if (argc != 2)
    {
        fprintf (stderr, RED_COLOR "Неправильный запуск программы (ввод аргументов командной строки)\n" RESET_COLOR);
        return -1;
    }

    char* buffer = GetFileContent (argv[1]);

    Node* main_node = GetTree (buffer);

    TreeDtor (main_node);
    free (buffer);
}
