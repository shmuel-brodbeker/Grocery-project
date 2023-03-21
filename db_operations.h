#ifndef _DB_OPERATIONS_H_
#define _DB_OPERATIONS_H_

#define INPUT_MAX_SIZE 250
#define NAME_LEN 20
#define PHONE_LEN 11

typedef struct _List {
    char first_name[NAME_LEN];
    char last_name[NAME_LEN];
    char phone[PHONE_LEN];
    struct _List *next;
    int id;
    int debt;
    unsigned short date[3]; // "01/01/2023"
} List;

enum FIELDS {
    FIRST_N,
    LAST_N,
    ID,
    PHONE,
    DEBT,
    DATE,
    SUM_FIELDS
};

typedef union FieldInfo
{
    char name[NAME_LEN];
    char phone[PHONE_LEN];
    int id;
    int debt;
    unsigned short date[3];
} FieldInfo;

typedef struct _Select {
    enum FIELDS field;
    char parameter;
    FieldInfo fieldInfo;
} Select;

void print_node(List *head);
void print_list (List *head);
int printing_approved (Select *pro_query, List *head);
void add_to_list (List *row, List **head);
List *is_id_exist(List *row, List **head);
void read_file (FILE *file, List **head);
void free_list (List *head);


#endif