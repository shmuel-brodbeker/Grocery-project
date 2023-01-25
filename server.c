#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db_operations.h"
#include "input_processing.h"

#define LINE "============="

int main (int argc, char **argv)
{
    List *head = NULL, *tail = NULL;

    if (argc < 2)
    {
        printf("Database not fount. usage: %s <db name>\n", argv[0]);
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

    // ------------------------------------------
    printf("\n%s List of debt %s\n", LINE, LINE);
    print_list(head);
    printf("%s%s%s\n", LINE, LINE, LINE);
    // ------------------------------------------
    
    free_list(head);
    
    return 0;
}