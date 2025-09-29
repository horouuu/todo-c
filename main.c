#include <stdio.h>
#include <string.h>
#include "./writer/writer.h"
#include "./reader/reader.h"

char *FILE_NAME = ".todo/test.txt";

int main(int argc, char **argsv)
{
    if (argc == 1)
    {
        return 0;
    }

    if (strcmp(argsv[1], "a") == 0)
    {
        todo_add(FILE_NAME, "hi!!!!", "1");
    }
    else if (strcmp(argsv[1], "b") == 0)
    {
        todo_delete(FILE_NAME, "10");
    }
    else if (strcmp(argsv[1], "c") == 0)
    {
        todo_display_all(FILE_NAME, DEFAULT);
    }

    return 0;
}