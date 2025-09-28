#include <stdio.h>
#include "./writer/writer.h"
#include "./reader/reader.h"

int main()
{
    todo_add(".todo/test.txt", "refactor code", 99);
    return 0;
}