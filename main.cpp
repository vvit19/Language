#include <cstdio>
#include <cstdlib>

const int MAX_CMD_LENGTH = 300;

int main (int argc, char** argv)
{
    char command[MAX_CMD_LENGTH] = "";

    sprintf (command, "cd frontend && make && ./front ../examples/%s.vit",
             argv[1]);
    system  (command);

    sprintf (command, "cd backend && make && ./back ../examples/%s.asm",
             argv[1]);
    system  (command);

    sprintf (command, "cd processor/asm && make && ./run ../../examples/%s.asm ../../examples/%s.bin",
             argv[1], argv[1]);
    system  (command);

    sprintf (command, "cd processor/spu && make && ./run ../../examples/%s.bin",
             argv[1]);
    system (command);
}
