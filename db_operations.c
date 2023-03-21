#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db_operations.h"
#include "input_processing.h"
#include "check_functions.h"

void free_list(List *head)
{
    if (!head)
        return;
    free_list(head->next);
    free(head);
}

void print_node(List *head)
{
    if (!head)
        return;
    printf("%s %s, %s, ID: %09d\n  ", head->first_name, head->last_name, head->phone, head->id);
    printf("\t-> debt: %d\n\t-> date: %d/%d/%d \n", head->debt, head->date[0], head->date[1], head->date[2]);
}

void print_list(List *head)
{
    if (!head)
        return;
    print_node(head);
    print_list(head->next);
}

int need_to_print(int res, char parameter)
{
    // Gets a comparison result
    // and returns if to print
    switch (parameter)
    {
        case '<':
            return (res < 0) ? 0 : 1;
        case '>':
            return (res > 0) ? 0 : 1;
        case '=':
            return (res == 0) ? 0 : 1;
        case '!':
            return (res != 0) ? 0 : 1;
        default:
            return 1;
    }
}

int printing_approved(Select *s, List *head)
{
    if (!head)
        return 1;

    switch (s->field)
    {
        case FIRST_N:
            if (!need_to_print(strcmp(head->first_name, s->fieldInfo.name), s->parameter))
                return 0;
            break;
        case LAST_N:
            if (!need_to_print(strcmp(head->last_name, s->fieldInfo.name), s->parameter))
                return 0;
            break;
        case ID:
            if (!need_to_print(head->id - s->fieldInfo.id, s->parameter))
                return 0;
            break;
        case PHONE:
            if (!need_to_print(strcmp(head->phone, s->fieldInfo.name), s->parameter))
                return 0;
            break;
        case DEBT:
            if (!need_to_print(head->debt - s->fieldInfo.debt, s->parameter))
                return 0;
            break;
        case DATE:
            if (!need_to_print(cmp_date(head->date, s->fieldInfo.date), s->parameter))
                return 0;
    }
    return 1;
}

void update_row(List *a, List *b)
{
    a->debt += b->debt;
    if (cmp_date(a->date, b->date) < 0)
    {
        memcpy(a->date, b->date, sizeof(b->date));
        memcpy(a->phone, b->phone, sizeof(b->phone));
    }
    if (strcmp(a->first_name, b->first_name) || strcmp(a->last_name, b->last_name))
        printf("Warning! found different names for id %09d\n", a->id);
}

List *is_id_exist(List *row, List **head)
{
    List **pNext = head;
    List *temp;

    if (!row)
        return NULL;

    while (*pNext)
    {
        if (row->id == (*pNext)->id)
        {
            update_row(row, *pNext);
                temp = *pNext;
                *pNext = (*pNext)->next;
                free(temp);
            return row;
        }
        pNext = &((*pNext)->next);
    }
    return row;
}

void add_to_list(List *row, List **head)
{
    List **temp = head;

    if (!row)
    {
        return;
    }

    if (*head == NULL || (*head)->debt > row->debt)
    {
        row->next = *head;
        *head = row;
    }
    else
    {
        while (*temp && (*temp)->debt < row->debt)
        {
            temp = &((*temp)->next);
        }
        row->next = *temp;
        *temp = row;
    }
}

void read_file(FILE *file, List **head)
{
    char input[INPUT_MAX_SIZE] = {0};
    List *row = NULL;

    while (fgets(input, sizeof(input), file))
    {
        row = processing_file(input, sizeof(input));
        if (row)
        {
            row = is_id_exist(row, head);
            add_to_list(row, head);
        }
    }
}
