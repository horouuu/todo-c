#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <time.h>
#include <direct.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include "../reader/reader.h"

void todo_create_directory(char *dir_name)
{
#ifdef _WIN32
    _mkdir(dir_name);
#else
    mkdir(dir_name, 0755);
#endif
}

int todo_create_file(const char *file_name, FILE **fptr)
{
    *fptr = fopen(file_name, "w+");
    if (!*fptr)
    {
        return 1;
    }
    fprintf(*fptr, "%-12u\n", 1);
    fflush(*fptr);
    rewind(*fptr);

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

    out_size = strlen(timestamp_string) + strlen(todo_message) + strlen(priority) + strlen(id) + 5 + 1 + 1 + 1;
    *out = malloc(out_size);

    if (!*out)
    {
        return 1;
    }

    sprintf(*out, "%s,%s,\"%s\",%s,0,0", id, timestamp_string, todo_message, priority);
    return 0;
}

int todo_add(const char *file_name, const char *todo_message, const char *priority)
{
    FILE *fptr;
    time_t timestamp;
    char curr_id_raw[13];
    char *curr_id_raw_ptr = curr_id_raw;
    char curr_id[12];
    char *curr_id_ptr = curr_id;
    unsigned int curr_available_id;
    int ctstatus;
    int cfstatus;
    char *todo_string;
    char *fgstatus;

    fptr = fopen(file_name, "r+");
    if (!fptr)
    {
        todo_create_directory(".todo");
        cfstatus = todo_create_file(file_name, &fptr);
        if (cfstatus != 0)
        {
            printf("Error creating file.");
            return 1;
        }
    }

    fgstatus = fgets(curr_id_raw, sizeof(curr_id_raw), fptr);
    if (!fgstatus)
    {
        printf("Failed to find id. Defaulting to 1.\n");
        strcpy(curr_id_raw, "1");
    }

    time(&timestamp);
    while (!isspace(*curr_id_raw_ptr) && *curr_id_raw_ptr != '\0')
    {
        *curr_id_ptr = *curr_id_raw_ptr;
        curr_id_ptr++;
        curr_id_raw_ptr++;
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
        printf("Error: todo message too long. Maximum: 60 bytes.\n");
        return 1;
    }
    else
    {
        printf("Adding entry:\n%s\n", todo_string);
    }

    curr_available_id += 1;
    rewind(fptr);
    fprintf(fptr, "%-12u\n", curr_available_id);

    fseek(fptr, 0, SEEK_END);
    fprintf(fptr, "%s\n", todo_string);

    fclose(fptr);
    free(todo_string);
    return 0;
}

int todo_delete(const char *file_name, const char *id)
{
    FILE *fptr;
    char buff[106];
    char id_buff[12];
    char *buff_ptr = buff;
    char *id_buff_ptr = id_buff;
    int found = 0;
    char deleted;
    int status_buff;

    fptr = fopen(file_name, "r+");
    if (!fptr)
    {
        fprintf(stderr, "Error deleting todo: file not found.\n");
        return 1;
    }

    if (fgets(buff, 14, fptr) == NULL)
    {
        fprintf(stderr, "Corrupted or empty file.\n");
        fclose(fptr);
        return 1;
    }

    while (fgets(buff, 106, fptr))
    {
        id_buff_ptr = id_buff;
        buff_ptr = buff;
        while (*buff_ptr != ',' && *buff_ptr != '\n')
        {
            *id_buff_ptr = *buff_ptr;
            buff_ptr++;
            id_buff_ptr++;
        }
        *id_buff_ptr = '\0';
        if (strcmp(id_buff, id) == 0)
        {
            printf("Found matching id %s\n", id);
            found = 1;
            break;
        }
    }

    if (!found)
    {
        printf("No todos with the id %s was found.\n", id);
        return 0;
    }

    status_buff = fseek(fptr, -3, SEEK_CUR);
    if (status_buff != 0)
    {
        fprintf(stderr, "Something went wrong: fseek(fptr, -3, SEEK_CUR)\n");
        fclose(fptr);
        return 1;
    }
    deleted = (char)fgetc(fptr);
    if (deleted == EOF)
    {
        fprintf(stderr, "fgetc returned invalid value or no characters are present to be read.\n");
        fclose(fptr);
        return 1;
    }
    status_buff = fseek(fptr, -1, SEEK_CUR);
    if (status_buff != 0)
    {
        fprintf(stderr, "Something went wrong: fseek(fptr, -3, SEEK_CUR)\n");
        fclose(fptr);
        return 1;
    }

    if (deleted == '1')
    {
        printf("Entry with id %s already marked for deletion.\n", id);
    }
    else if (deleted == '0')
    {
        fputc((int)'1', fptr);
        printf("Successfully marked entry with id %s for deletion.\n", id);
    }
    else
    {
        printf("Error: Invalid value for deletion status.\n");
        return 1;
    }

    fclose(fptr);
    return 0;
}