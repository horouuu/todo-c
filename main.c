#include <stdio.h>
#include <string.h>
#include "./writer/writer.h"
#include "./reader/reader.h"

int main(int argc, char **argsv)
{
    if (argc == 1)
    {
        return 0;
    }

    if (strcmp(argsv[1], "a") == 0)
    {
        todo_add(".todo/test.txt", "hi!!!!", "1");
    }
    else
    {
        todo_delete(".todo/test.txt", "10");
    }

    return 0;
}