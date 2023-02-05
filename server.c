#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "db_operations.h"
#include "input_processing.h"

#define MAX_LEN 1024
#define MIN_OF(_a, _b) ((_a < _b) ? _a : _b)
#define LINE "=============" 

struct Server_param
{
    List *head;
    int sock;
};

int insert_node_to_buf (char *buf, int rest, List *node)
{
    char x[MAX_LEN] = {0};
    snprintf(x, MAX_LEN, "%s %s, %s, ID: %09d\n\t-> debt: %d \n\t-> date: %d/%d/%d \n",
        node->first_name, node->last_name, node->phone, node->id,
        node->debt, node->date[0], node->date[1], node->date[2]);
    strncpy(buf, x, MIN_OF(strlen(x), rest)); 
    return strlen(x); 
}

void send_selection (List *head, int socket, Select *selection, int *counter)
{
    char buf [MAX_LEN];
    int len, n;
    while (head)
    {
        memset (buf, 0, sizeof(buf));
        for (int i = 0, len = 0; i < 5; i++) // send 5 lines together
        {
            if (!head)
                break;

            if (printing_approved(selection, head) == 0)
            {
                len += insert_node_to_buf(buf + len, MAX_LEN - len, head); 
                (*counter)++;
            }
            head = head->next;
        }
        n = send (socket, buf, strlen(buf), 0);
        if (n < 0)
        {
            perror("Server error sending data");
            close(socket);
        }
    }
}

void send_list (struct Server_param *sp)
{
    List *p_head = sp->head;
    char buf [MAX_LEN];
    int len, n;
    while (p_head)
    {
        memset (buf, 0, sizeof(buf));
        for (int i = 0, len = 0; i < 5; i++) // send 5 lines together
        {
            if (!p_head)
                break;
            len += insert_node_to_buf(buf + len, MAX_LEN - len, p_head); 
            p_head = p_head->next;
        }
        n = send (sp->sock, buf, strlen(buf), 0);
        if (n < 0)
        {
            perror("Server error sending data");
            close(sp->sock);
        }
    }
}

void send_for_processing (char *buffer, int max_len, struct Server_param *sp)
{
    char command[30] = {0};
    sscanf(buffer, "%29s", command);

    if (!strcmp(command, "select"))
    {
        if (!sp->head)
        {
            snprintf(buffer, MAX_LEN, "List is empty\n");
            return;
        }
        Select *selection = check_select_query (buffer + strlen(command) + 1);
        if (selection)
        {
            int counter = 0;
            send_selection (sp->head, sp->sock, selection, &counter);
            
            if (counter == 0)
                snprintf(buffer, MAX_LEN, "No data found to display\n");
            else
                snprintf(buffer, MAX_LEN, "\n============ End list ===========\n");
        }
        else
        {
            strcpy(buffer, buffer + strlen(command) + 1);
        }
    }
    else if (!strcmp(command, "set"))
    {
        List *new = add_new_row(buffer + strlen(command) + 1);
        if (new)
        {
            new = is_id_exist(new, &sp->head);
            add_to_list (new, &sp->head);
            snprintf(buffer, MAX_LEN, "New record added successfully\n");
            return;
        }
        strcpy(buffer, buffer + strlen(command) + 1);
    }
    else if (!strcmp(command, "print"))
    {
        if (!sp->head)
        {
            snprintf(buffer, MAX_LEN, "List is empty\n");
            return;
        }
        send_list (sp);
        snprintf(buffer, MAX_LEN, "\n============ End list ===========\n");
    }
    else
    {
        snprintf(buffer, MAX_LEN, "Query word unidentified. Usage: select, set, print, quit\n");
    }
}

void *conn_handler(void *args)
{
    struct Server_param *sp = args;
    char buffer[MAX_LEN];
    int n;

    n = recv(sp->sock, buffer, MAX_LEN, 0);
    if (n < 0)
    {
        perror("Server error receiving data");
        goto exit;
    }
    buffer[n] = '\0';
    send_for_processing(buffer, MAX_LEN, sp); 

    n = send(sp->sock, buffer, strlen(buffer), 0);
    if (n < 0)
    {
        perror("Server error sending data");
        goto exit;
    }
exit:
    close(sp->sock);
    return NULL;
}

int get_query (struct Server_param *sp)
{
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t len = sizeof(client_addr);

    /* Create a socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 1;
    }

    /* Bind the socket to a specific port */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(sp->sock);
    if (bind(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("Error binding socket");
        return 1;
    }

    if (listen(sockfd, 5) < 0)
    {
        perror("Error listening");
        return 1;
    }

    /* Receive data from clients */
    while (1)
    {
        pthread_t tid;
        sp->sock = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t*)&len);
        if (sp->sock < 0)
        {
            perror("accept failed");
            return 1;
        }

        pthread_create(&tid, NULL, conn_handler, (void *)sp); 
        pthread_join(tid, NULL);
    }
    return 0;
}

int main (int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Usage: %s <db.csv> <port>\n", argv[0]);
        return 1;
    }
    
    struct Server_param sp = {
        .head = NULL,
        .sock = atoi(argv[2])
    };
    
    FILE *file = fopen(argv[1], "r");
    if (!file)
    {
        puts("File not found");
        return 1;
    }
    read_file(file, &sp.head);
    fclose(file);

    // ----------- print in server --------------
    printf("\n%s List of debt %s\n", LINE, LINE);
    print_list(sp.head);
    printf("%s%s%s\n", LINE, LINE, LINE);
    // ------------------------------------------

    get_query(&sp);
    
    free_list(sp.head); // their is not an exit function
    
    return 0;
}