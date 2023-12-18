#include "utils.h"

char* GetFileContent(const char* filename)
{
    assert(filename);

    FILE* file = fopen(filename, "rb");

    int file_size = GetFileSize(file);

    char* buffer = (char*) calloc(file_size + 1, sizeof(char));
    assert(buffer);

    buffer[file_size] = '\0';
    fread(buffer, sizeof(char), file_size, file);

    fclose(file);

    return buffer;
}

int GetFileSize(FILE* file)
{
    assert(file);

    fseek(file, 0, SEEK_END);
    int position = ftell(file);
    fseek(file, 0, SEEK_SET);

    return position;
}

int CalcNlines(char* buffer)
{
    assert(buffer);

    int nlines = 0;
    char* temp = buffer;

    while ((temp = strchr(temp, '\n')) != nullptr)
    {
        nlines++;
        temp++;
    }

    return nlines;
}

bool IsEqual(double a, double b)
{
    const double EPS = 1e-10;
    return fabs(a - b) < EPS;
}

int SkipSpaces (char* buffer, int i)
{
    assert (buffer);

    while (isspace (buffer[i])) i++;

    return i;
}

int Factorial (int n)
{
    if (n == 1 || n == 0) return 1;
    return n * Factorial (n - 1);
}

bool IsInt (double n)
{
    return IsEqual (n, round (n));
}
