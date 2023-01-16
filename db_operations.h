#ifndef _DB_OPERATIONS_H_
#define _DB_OPERATIONS_H_

typedef struct _List {
    char first_name[15];
    char last_name[15];
    char phone[11];
    struct _List *next;
    int date[3]; // "01/01/2023"
    int id;
    int debt;
} List;

typedef struct _Select {
    int field;
    int to_test_num[3];
    char parameter;
    char to_test_str[20];
} Select;

enum FIELDS {
    FIRST_N,
    LAST_N,
    ID,
    PHONE,
    DEBT,
    DATE
};

void print_list (List *head);
void print_query (Select *pro_query, List *head);
void add_to_list (List *row, List **head);
List *is_id_exist(List *row, List **head);

// void build_sort_list (List*, Sort_list**, Sort_list**);
// void print_sort_list(Sort_list*);



#endif