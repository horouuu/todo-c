#ifndef READER_H
#define READER_H

typedef enum
{
    DEFAULT,
    PRIORITY,
    TIME_CREATED,
    DONE,
    MARKED
} todo_sort_t;

/* Displays all todos in a numbered list. */
int todo_display_all(char* file_name, todo_sort_t sort);

#endif