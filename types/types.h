#ifndef TYPES_H
#define TYPES_H
#include <time.h>

typedef struct {
    time_t timestamp;
    char* content;
    unsigned int priority;
    unsigned int id;
} Todo;

typedef struct
{
    unsigned int curr_available_id;
    Todo *data;
} TodoCollection;

#endif