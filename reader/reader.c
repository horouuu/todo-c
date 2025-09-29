#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reader.h"

/* Advances start_ptr until the next comma, newline, or EOF. */
int parse_line(char *out, const int out_size, const char *line, char **start_ptr, FILE *fptr)
{
    size_t len;
    char *start_ptr_temp;
    start_ptr_temp = *start_ptr;

    while (**start_ptr && **start_ptr != ',' && **start_ptr != '\n' && **start_ptr != EOF)
    {
        (*start_ptr)++;
    }

    if (**start_ptr != ',' && **start_ptr != '\n')
    {
        return 0;
    }

    len = (size_t)(*start_ptr - start_ptr_temp);
    if (len >= out_size)
    {
        fprintf(stderr, "Given output buffer size %d too small for output (size %d).\n", out_size, len);
        return -1;
    }

    memcpy(out, start_ptr_temp, len);
    out[len] = '\0';
    return 0;
}

int todo_display_all(char *file_name, todo_sort_t sort)
{
    FILE *fptr;
    char id_buff[14];
    char todo_buff[106];
    char *todo_display_buff = malloc(100);

    char *pos_ptr;
    unsigned long id;

    char ts_buff[21];
    char *ts_buff_ptr = ts_buff;

    char msg_buff[61];
    char *msg_buff_ptr = msg_buff;

    char priority_buff[13];
    char *priority_buff_ptr = priority_buff;

    char done_buff[2];
    char *done_buff_ptr = done_buff;

    char deleted_buff[2];
    char *deleted_buff_ptr = deleted_buff;

    fptr = fopen(file_name, "r");

    if (!fptr)
    {
        fprintf(stderr, "Storage file not found.\n");
        return 1;
    }

    if (!fgets(id_buff, 14, fptr))
    {
        fprintf(stderr, "Corrupted or empty file.\n");
        fclose(fptr);
        return 1;
    }

    while (fgets(todo_buff, 106, fptr))
    {
        id = strtoul(todo_buff, &pos_ptr, 10);
        if (*pos_ptr != ',')
        {
            fprintf(stderr, "Something went wrong when parsing ID.\n");
            fclose(fptr);
            return 1;
        }
        pos_ptr++;

        if (parse_line(ts_buff_ptr, 32, todo_buff, &pos_ptr, fptr) != 0)
        {
            fclose(fptr);
            printf("Something went wrong when parsing timestamp.\n");
            return 1;
        }
        pos_ptr++;

        if (parse_line(msg_buff_ptr, 60, todo_buff, &pos_ptr, fptr) != 0)
        {
            fclose(fptr);
            printf("Something went wrong when parsing message.\n");
            return 1;
        }
        pos_ptr++;

        if (parse_line(priority_buff_ptr, 20, todo_buff, &pos_ptr, fptr) != 0)
        {
            fclose(fptr);
            printf("Something went wrong when parsing priority.\n");
            return 1;
        }
        pos_ptr++;

        if (parse_line(done_buff_ptr, 2, todo_buff, &pos_ptr, fptr) != 0)
        {
            fclose(fptr);
            printf("Something went wrong when parsing done marker.\n");
            return 1;
        }
        pos_ptr++;

        if (parse_line(deleted_buff_ptr, 2, todo_buff, &pos_ptr, fptr) != 0)
        {
            fclose(fptr);
            printf("Something went wrong when parsing deleted marker.\n");
            return 1;
        }
        printf("%u. %s | %s (%s)\n", id, msg_buff, ts_buff, priority_buff);
    }

    free(todo_display_buff);
    fclose(fptr);

    return 0;
}