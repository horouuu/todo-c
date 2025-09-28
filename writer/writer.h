#ifndef WRITER_H
#define WRITER_H
#include <time.h>

int todo_add(const char *file_name, const char *todo_message, const unsigned int priority);
int create_todo(const char *todo_message, const unsigned int priority, const unsigned int id, const time_t timestamp, char **out);

#endif