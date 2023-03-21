#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "check_functions.h"
#include "db_operations.h"

#define MIN_OF(_a, _b) ((_a < _b) ? _a : _b)

void skip_spaces(char **p)
{
    while(**p == ' ')
        (*p)++;
}

List *processing_file (char *input, int size)
{
    List *node;
    char *start_field = input, *p = input;
    int active_field = FIRST_N;
    int len = 0;

    node = calloc(1, sizeof(List));
    if (!node)
    {
        printf("Memory allocation error. try again\n");
        return NULL; 
    }

    for (int i = 0; i < size; i++, p++, len++)
    {
        // P run on the string until fount a comma
        if (*p == '\n' && active_field < DATE)
        {
            goto err;
        }

        if (*p == ',' || *p == '\n' && active_field == DATE)
        {
            switch (active_field)
            {
                case FIRST_N:
                    strncpy(node->first_name, start_field, MIN_OF(len, sizeof(node->first_name))); // dynamic, merge rows
                    break;
                case LAST_N:
                    strncpy(node->last_name, start_field, MIN_OF(len, sizeof(node->last_name)));
                    break;
                case ID:
                    sscanf(start_field ,"%d", &node->id);
                    if (len != 9 || node->id <= 0 || check_id(start_field))
                    {
                        printf("Warning! wrong ID %d, This row cannot be accepted.\n", node->id);
                        goto err;
                    }
                    break;
                case PHONE:
                    strncpy(node->phone, start_field, len);
                    break;
                case DEBT:
                    sscanf(start_field ,"%d", &node->debt);
                    if (check_debt(start_field, len-1) || node->debt == 0)
                    {
                        printf("Warning! wrong debt for ID %d, This row cannot be accepted.\n", node->id);
                        goto err;
                    }
                    break;
                case DATE:
                    sscanf(start_field ,"%hu%*c%hu%*c%hu", &node->date[0], &node->date[1],&node->date[2]);
                    break;
           }
            active_field++;
            start_field = p + 1;
            len = -1;
        }

        if (*p == '\n')
        {
            break;
        }
    }

    if (check_node(node))
    {
        goto err;
    }
    return node;

err:
    free(node);
    return NULL;
}

int find_filed (char **p)
{
    char *fields[] = {"first name", "last name", "id", "phone", "debt", "date"};
    char field[11] = {0};
    char *np = *p;
    int len = 0;

    while ((*np >= 'a' && *np <= 'z') || *np == ' ')
    {
        np++; len++;
    }

    if (*np == '\n' || *np == ',')
        return -1;

    strncpy(field, *p, MIN_OF(len, sizeof(field)));
    *p = np;

    if (!strncmp(field, fields[0], strlen(fields[0])))
        return FIRST_N;
    else if (!strncmp(field, fields[1], strlen(fields[1])) 
        || !strcmp(field, "second name"))
        return LAST_N;
    else if (!strncmp(field, fields[2], strlen(fields[2])))
        return ID;
    else if (!strncmp(field, fields[3], strlen(fields[3])))
        return PHONE;
    else if (!strncmp(field, fields[4], strlen(fields[4])))
        return DEBT;
    else if (!strncmp(field, fields[5], strlen(fields[5])))
        return DATE;
    else
        return -1;
}

Select *check_select_query (char *input)
{
    Select *query = calloc(1, sizeof(Select));
    if (!query)
    {
        snprintf(input, 40, "An error has occurred, try again.\n");
        return NULL; 
    }
    
    char *p = input;
    skip_spaces(&p);

    query->field = find_filed(&p);
    if (query->field < 0)
    {
        snprintf(input, 40, "Error. field name unidentified.\n");
        goto err;
    }
    skip_spaces(&p);

    query->parameter = *p;
    if (*p == '!')
        p++;    

    p++;
    skip_spaces(&p);

    switch(query->field)
    { 
        case FIRST_N:
            sscanf(p, "%19[^\t\n]", query->fieldInfo.name);
            if (check_name(query->fieldInfo.name))
            {
                snprintf(input, 20, "Invalid name\n");
                goto err;
            } 
            break;
        case LAST_N:
            sscanf(p, "%19[^\t\n]", query->fieldInfo.name);
            if (check_name(query->fieldInfo.name))
            {
                snprintf(input, 20, "Invalid name\n");
                goto err;
            }
            break;
        case ID:
            sscanf(p, "%d", &query->fieldInfo.id);
            if (query->fieldInfo.id < 0)
            {
                snprintf(input, 20, "Invalid ID\n");
                goto err;
            }
            break;
        case PHONE:
            sscanf(p, "%19[^\t\n]", query->fieldInfo.phone);
            if (check_phone (query->fieldInfo.phone))
            {
                snprintf(input, 20, "Invalid phone\n");
                goto err;
            }
            break;
        case DEBT:
            if (*p && *p != '-' && (*p < '0' || *p > '9'))
            {
                snprintf(input, 20, "Wrong debt.\n");
                goto err;
            }            
            sscanf(p, "%d", &query->fieldInfo.debt);
            break;
        case DATE:
            sscanf(p, "%hd%*c%hd%*c%hd", &query->fieldInfo.date[0], 
                    &query->fieldInfo.date[1], &query->fieldInfo.date[2]);
            if (check_date(query->fieldInfo.date))
            {
                snprintf(input, 40, "Invalid date. usage: dd/mm/yyyy\n");
                goto err;
            }
            break;
        default:
            snprintf(input, 80, "Error. usage: <field name> <parameter: <, >, =, != > <your selection>\n");
            goto err;
    }
    return query;

err:
    free(query);
    return NULL;
}

List *add_new_row (char *input)
{
    List *new = calloc(1, sizeof(List));
    if (!new)
    {
        puts("Error");
        return NULL;
    }
    char *p = input;
    char *start = input;
    int sum_fields = 6;
    int cur_field = 0;
    int len = 0;

    for (int i = 0; i < sum_fields; i++, p++)
    {
        skip_spaces(&p);
        cur_field = find_filed(&p);
        // p point now to '='

        p++;
        skip_spaces(&p);

        start = p;
        len = 0;
        
        while (*p && *p != ',') 
        {
            if (*p == '\n' && i < 5 )
            {
                snprintf(input, 20, "Too few fields\n");
                goto err;
            }
            p++; len++;
        }
        if (len == 0) 
        {
            snprintf(input, 25, "Error. empty field\n");
            goto err;
        }
        switch (cur_field)
        {
            case FIRST_N:
                strncpy(new->first_name, start, len);
                break;
            case LAST_N:
                strncpy(new->last_name, start, len);
                break;
            case ID:
                sscanf(start ,"%d", &new->id);
                if (len != 9 || new->id <= 0 || check_id(start))
                {
                    snprintf(input, 60, "Warning! wrong ID %d, This row cannot be accepted.\n", new->id);
                    goto err;
                }
                break;
            case PHONE:
                strncpy(new->phone, start, len);
                break;
            case DEBT:
                new->debt = atoi(start);
                if (check_debt(start, len-1))
                {
                    snprintf(input, 70, "Wrong debt for ID %d, This row cannot be accepted.\n", new->id);
                    goto err;
                }
                if (new->debt == 0)
                {
                    snprintf(input, 70, "Debt for ID %d is not found, This row cannot be accepted.\n", new->id);
                    goto err;
                }
                break;
            case DATE:
                sscanf(start ,"%hu%*c%hu%*c%hu", &new->date[0], &new->date[1],&new->date[2]);
                break;
            default:
                snprintf(input, 40, "Error. field name unidentified.\n");
                goto err;
        }
        start = p+1;

    if (check_node(new))
    {
        goto err;
    }
    
    }
    return new;

err:
    free(new);
    return NULL;
}