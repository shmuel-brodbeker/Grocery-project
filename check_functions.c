#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "db_operations.h"

int check_name(char *str)
{
    if (strlen(str) == 0)
        return 1;

    for (int i = 0; i < strlen(str); i++)
    {
        // Checks for valid characters in a name
        if (str[i] >= 'a' && str[i] <= 'z')
            continue;

        if (str[i] == ' ' || str[i] == '_')
            continue;

        if (str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] += ('a' - 'A');
        }
        else
        {
            return 1;
        }
    }
    return 0;
}    

int check_phone (char *str)
{
    int len = strlen(str);
    
    if (str[0] != '0' || len != 10)
    {
        return 1;
    }
    for (int i = 0; i < len; i++)
    {
        if (str[i] < '0' || str[i] > '9')
        {
            return 1;
        }
    }
    return 0;
}

int check_id (char *start)
{
    if (atoi(start) <= 0)
        return 1;

    char *p = start;
    for (int i = 0; i < 9; i++, p++)
    {
        if (*p < '0' || *p > '9')
            return 1;
    }
    if (*p > '0' && *p < '9')
        return 1; // id can not be more of 9 digits

    return 0;
}

int check_debt (char *start, int len)
{
    char *p = start;
    for (int i = 0; i < len; i++, p++)
    {
        if (i == 0 && *p == '-' && len > 1)
            continue;
        
        if (*p < '0' || *p > '9')
            return 1;
    }
    return 0;
}

int check_date (unsigned short *date)
{
    unsigned short d = date[0], m = date[1], y = date[2];
    if (d < 0 || d > 31 || m < 0 || m > 12 || y < 1900 || y > 2200)
    {
        return 1;
    }
    return 0;
}

int check_node(List *node)
{
    return check_name(node->first_name) +
        check_name(node->last_name) +
        check_phone(node->phone) +
        check_date(node->date);
}

int cmp_date (short a[3], short b[3])
{
    if (a[2] > b[2])
        return 1;
    else if (a[2] < b[2])
        return -1;
    else // the same year
    {
        if (a[1] > b[1])
            return 1;
        else if (a[1] < b[1])
            return -1;
        else // the same month
        {
            if (a[0] > b[0])
                return 1;
            else if (a[0] < b[0])
                return -1;
            else // the same date
            {
                return 0;
            }
        }
    }
}