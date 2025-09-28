#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>
#include <direct.h>
#include <sys/stat.h>
#include <sys/types.h>

int create_todo(const char *todo_message, const unsigned int priority, const unsigned int id, const time_t timestamp, char **out)
{

    int len = 32;
    int curr_len = 0;
    size_t out_size;
    char timestamp_string[len];
    char priority_string[len];
    char id_string[len];
    snprintf(timestamp_string, len, "%" PRIdMAX, (intmax_t)timestamp);
    snprintf(priority_string, len, "%u", priority);
    snprintf(id_string, len, "%u", id);

    out_size = strlen(timestamp_string) + strlen(todo_message) + strlen(priority_string) + strlen(id_string) + 4 + 1 + 1;
    *out = malloc(out_size);

    if (!out)
    {
        return 1;
    }

    snprintf(*out, out_size, "%s,\"%s\",%s,%s,0", timestamp_string, todo_message, priority_string, id_string);
    return 0;
}

int todo_add(const char *file_name, const char *todo_message, const unsigned int priority)
{
    FILE *fptr;
    time_t timestamp;
    char curr_id_raw[5];
    unsigned int curr_available_id;
    int curr_pos;
    int ctstatus;
    char *todo_string;
    char *fgets_result;

    fptr = fopen(file_name, "r+");
    if (!fptr)
    {

#ifdef _WIN32
        _mkdir(".todo");
#else
        mkdir(".todo", 0755);
#endif

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