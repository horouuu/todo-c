#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>
#include <direct.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>

void todo_create_directory()
{
#ifdef _WIN32
    _mkdir(".todo");
#else
    mkdir(".todo", 0755);
#endif
}

int todo_create_file(const char *file_name, FILE *fptr)
{
    fptr = fopen(file_name, "w+");
    if (!fptr)
    {
        return 1;
    }
    fprintf(fptr, "%-12u\n", 1);
    fflush(fptr);
    rewind(fptr);
    printf("\n");

    return 0;
}

int todo_create(const char *todo_message, const char *priority, const char *id, const time_t timestamp, char **out)
{
    size_t out_size;
    char timestamp_string[32];

    if (strlen(todo_message) > 60)
    {
        return -1;
    }

    sprintf(timestamp_string, "%" PRIdMAX, (intmax_t)timestamp);

    out_size = strlen(timestamp_string) + strlen(todo_message) + strlen(priority) + strlen(id) + 4 + 1 + 1;
    *out = malloc(out_size);

    if (!*out)
    {
        return 1;
    }

    sprintf(*out, "%s,%s,\"%s\",%s,0", id, timestamp_string, todo_message, priority);
    return 0;
}

int todo_add(const char *file_name, const char *todo_message, const char *priority)
{
    FILE *fptr;
    time_t timestamp;
    char *curr_id_raw = malloc(13);
    char *curr_id = malloc(12);
    char *curr_id_ptr = curr_id;
    unsigned int curr_available_id;
    int ctstatus;
    int cfstatus;
    char *todo_string;
    char *fgets_result;

    fptr = fopen(file_name, "r+");
    if (!fptr)
    {
        todo_create_directory();
        cfstatus = todo_create_file(file_name, fptr);
        if (cfstatus != 0)
        {
            printf("Error creating file.");
            return 1;
        }
    }

    fgets_result = fgets(curr_id_raw, sizeof(curr_id_raw), fptr);
    if (!fgets_result)
    {
        strcpy(curr_id_raw, "1");
    }

    time(&timestamp);
    while (!isspace(*curr_id_raw) && *curr_id_raw != '\0')
    {
        *curr_id = *curr_id_raw;
        curr_id_ptr++;
        curr_id_raw++;
    }
    *curr_id_ptr = '\0';

    curr_available_id = strtol(curr_id, NULL, 0);
    if (!curr_available_id)
    {
        printf("Error reading ID: either non-integer or reserved value 0 found in file.\n");
        return 1;
    }

    ctstatus = todo_create(todo_message, priority, curr_id, timestamp, &todo_string);
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