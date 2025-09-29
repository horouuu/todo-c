#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>
#include <direct.h>
#include <sys/stat.h>
#include <sys/types.h>

void create_todo_directory()
{
#ifdef _WIN32
    _mkdir(".todo");
#else
    mkdir(".todo", 0755);
#endif
}

int create_todo(const char *todo_message, const unsigned int priority, const unsigned int id, const time_t timestamp, char **out)
{
    size_t out_size;
    char timestamp_string[32];
    char priority_string[32];
    char id_string[32];

    if (strlen(todo_message) > 60)
    {
        return -1;
    }

    sprintf(timestamp_string, "%" PRIdMAX, (intmax_t)timestamp);
    sprintf(priority_string, "%u", priority);
    sprintf(id_string, "%u", id);

    out_size = strlen(timestamp_string) + strlen(todo_message) + strlen(priority_string) + strlen(id_string) + 4 + 1 + 1;
    *out = malloc(out_size);

    if (!out)
    {
        return 1;
    }

    sprintf(*out, "%s,%s,\"%s\",%s,0", priority_string, timestamp_string, todo_message, id_string);
    return 0;
}

int todo_add(const char *file_name, const char *todo_message, const unsigned int priority)
{
    FILE *fptr;
    time_t timestamp;
    char curr_id_raw[12];
    unsigned int curr_available_id;
    int ctstatus;
    char *todo_string;
    char *fgets_result;

    fptr = fopen(file_name, "r+");
    if (!fptr)
    {
        create_todo_directory();
        fptr = fopen(file_name, "w+");
        if (!fptr)
            return 1;
        fprintf(fptr, "0\n");
        fflush(fptr);
        printf("\n");
    }

    rewind(fptr);
    fgets_result = fgets(curr_id_raw, sizeof(curr_id_raw), fptr);
    if (!fgets_result)
    {
        strcpy(curr_id_raw, "0");
    }

    time(&timestamp);
    curr_available_id = strtol(curr_id_raw, NULL, 0);
    ctstatus = create_todo(todo_message, priority, curr_available_id, timestamp, &todo_string);
    if (ctstatus != 0)
    {
        printf("Error.\n");
        return 1;
    }
    else if (ctstatus == -1)
    {
        printf("Error: todo message too long. Maximum: 60 characters.\n");
        return 1;
    }
    else
    {
        printf("Adding entry:\n%s\n", todo_string);
    }

    curr_available_id += 1;
    rewind(fptr);
    fprintf(fptr, "%u\n", curr_available_id);

    fseek(fptr, 0, SEEK_END);
    fprintf(fptr, "%s\n", todo_string);

    fclose(fptr);
    free(todo_string);
    return 0;
}