#include "parser.h"
#include "dump.h"

int main (int argc, const char** argv)
{
    if (argc != 2)
    {
        fprintf (stderr, RED_COLOR "Неправильный запуск программы (ввод аргументов командной строки)\n" RESET_COLOR);
        return -1;
    }

    char* buffer    = GetFileContent (argv[1]);
    Node* main_node = GetGrammar (buffer);
    free (buffer);

    if (main_node == nullptr)
    {
        fprintf (stderr, RED_COLOR "Выход из программы (синтаксическая ошибка)\n" RESET_COLOR);
        return -1;
    }


    GraphDump (main_node);
    ASTDump (main_node);

    TreeDtor (main_node);

    return 0;
}
