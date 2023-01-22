#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db_operations.h"
#include "input_processing.h"

#define LINE "============="

void get_query (List **head)
{
    char menu[] = {"Enter a query <quit for exit>"};
    char query[200];
    char command[30];

    while(1)
    {
        memset(query, 0, sizeof(query));
        memset(command, 0, sizeof(command));

        printf("\n%s\n-->", menu);
        fgets(query, sizeof(query), stdin);
        sscanf(query, "%s", command);

        if (!strcmp(command, "select"))
        {
            Select *pro_query = check_select_query (query + strlen(command) + 1);
            if (pro_query)
            {
                int counter = 0;
                print_query (pro_query, *head, &counter);
                if (counter == 0)
                    puts("No data found to display");
            }
        }
        else if (!strcmp(command, "set"))
        {
            List *new = add_new_row(query + strlen(command) + 1);
            if (new)
            {
                new = is_id_exist(new, head);
                add_to_list (new, head);
            }
        }
        else if (!strcmp(command, "print"))
        {
            print_list(*head);
        }
        else if (!strcmp(command, "quit"))
        {
            break;
        }
        else
        {
            puts("Error - query word");
            puts("Usage: select, set, print, quit.");
        }
    }
}

void read_file (FILE *file, List **head, List **tail)
{
    char input[250] = {0};
    List *row = NULL;

    while (fgets(input, sizeof(input), file))
    {
        row = processing_file(input, sizeof(input));
        if (row)
        {
            row = is_id_exist(row, head);
            add_to_list (row, head);
        }
    }
}

int main (int argc, char **argv)
{
    List *head = NULL, *tail = NULL;

    if (argc < 2)
    {
        puts("Error. no database file name.");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file)
    {
        puts("File not found");
        return 1;
    }
    read_file(file, &head, &tail);
    fclose(file);

    printf("\n%s List of debt %s\n", LINE, LINE);
    print_list(head);
    printf("%s%s%s\n", LINE, LINE, LINE);

    get_query(&head);

    // list_to_file(head, argv[1]);
    free_list(head);
    
    return 0;
}