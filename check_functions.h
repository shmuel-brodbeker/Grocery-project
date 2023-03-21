#ifndef _CHECK_FUNCTIONS_H_
#define _CHECK_FUNCTIONS_H_

#include "db_operations.h"

int check_node(List *node);

int check_name(char *str);
int check_phone (char *str);
int check_id (char *start);
int check_debt (char *start, int len);
int check_date (unsigned short *date);

int cmp_date (short a[3], short b[3]);

#endif