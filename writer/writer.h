#ifndef WRITER_H
#define WRITER_H
#include <time.h>

int todo_add(const char *file_name, const char *todo_message, const char *priority);
int todo_create(const char *todo_message, const char *priority, const char *id, const time_t timestamp, char **out);
int todo_delete(const char *file_name, const char id);
void todo_create_directory();
void todo_create_file(const char *file_name, FILE *fptr);

#endif