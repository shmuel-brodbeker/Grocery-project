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

List *head = NULL; // global head

int insert_node_to_buf (char *buf, int rest, List *node)
{
    char x[MAX_LEN] = {0};
    snprintf(x, MAX_LEN, "%s %s, %s, ID: %09d\n\t-> debt: %d \n\t-> date: %d/%d/%d \n",
        node->first_name, node->last_name, node->phone, node->id,
        node->debt, node->date[0], node->date[1], node->date[2]);
    strncpy(buf, x, MIN_OF(strlen(x), rest)); 
    return strlen(x); 
}

// void insert_selection_to_buf (char *buf, int buf_len, Select *selection, int *counter)
// {
//     List *temp = head;
//     int len = 0;
//     memset (buf, 0, buf_len);

//     while (temp)
//     {
//         if (printing_approved(selection, temp) == 0)
//         {
//             len += insert_node_to_buf (buf + len, buf_len - len, temp);
//             (*counter)++;
//         }
//         temp = temp->next;
//     }
//     buf[len] = '\0';
// }

// void insert_list_to_buf (char *buf, int buf_len)
// {
//     List *temp = head;
//     int len = 0;
//     memset (buf, 0, buf_len);

//     while (temp)
//     {
//         len += insert_node_to_buf(buf + len, buf_len - len, temp); 
//         temp = temp->next;
//     }
//     buf[len] = '\0';
// }

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

void send_list (List *head, int socket)
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
            len += insert_node_to_buf(buf + len, MAX_LEN - len, head); 
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

void send_for_processing (char *buffer, int max_len, int socket)
{
    char command[30] = {0};
    sscanf(buffer, "%s", command);

    if (!strcmp(command, "select"))
    {
        if (!head)
        {
            snprintf(buffer, MAX_LEN, "List is empty\n");
            return;
        }
        Select *selection = check_select_query (buffer + strlen(command) + 1);
        if (selection)
        {
            int counter = 0;
            // insert_selection_to_buf (buffer, MAX_LEN, selection, &counter);
            send_selection (head, socket, selection, &counter);
            
            if (counter == 0)
                snprintf(buffer, MAX_LEN, "No data found to display\n");
            else
                snprintf(buffer, MAX_LEN, "\n============ End list ===========\n");
        }
        else
        {
            snprintf(buffer, MAX_LEN, "Error. Usage: <field name> <parameter: <, >, =, != > <your selection>\n");
            return;
        }
    }
    else if (!strcmp(command, "set"))
    {
        List *new = add_new_row(buffer + strlen(command) + 1);
        if (new)
        {
            new = is_id_exist(new, &head);
            add_to_list (new, &head);
            snprintf(buffer, MAX_LEN, "New record added successfully\n");
            return;
        }
        snprintf(buffer, MAX_LEN, "Error. New record not added\n");
    }
    else if (!strcmp(command, "print"))
    {
        if (!head)
        {
            snprintf(buffer, MAX_LEN, "List is empty\n");
            return;
        }
        // insert_list_to_buf (buffer, MAX_LEN);
        send_list (head, socket);
        snprintf(buffer, MAX_LEN, "\n============ End list ===========\n");
    }
    else
    {
        snprintf(buffer, MAX_LEN, "Query word unidentified. Usage: select, set, print, quit\n");
    }
}

void *conn_handler(void *args)
{
    char buffer[MAX_LEN];
    int n;
    int new_sock = (int)args; 

    n = recv(new_sock, buffer, MAX_LEN, 0);
    if (n < 0)
    {
        perror("Server error receiving data");
        goto exit;
    }
    buffer[n] = '\0';
    send_for_processing(buffer, MAX_LEN, new_sock); 

    n = send(new_sock, buffer, strlen(buffer), 0);
    if (n < 0)
    {
        perror("Server error sending data");
        goto exit;
    }
exit:
    close(new_sock);
    return NULL;
}

int get_query (int port)
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
    server_addr.sin_port = htons(port);
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
        int new_sock = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t*)&len);
        if (new_sock < 0)
        {
            perror("accept failed");
            return 1;
        }

        pthread_create(&tid, NULL, conn_handler, (void *)new_sock);
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
    
    FILE *file = fopen(argv[1], "r");
    if (!file)
    {
        puts("File not found");
        return 1;
    }
    read_file(file, &head);
    fclose(file);

    // ------------------------------------------
    printf("\n%s List of debt %s\n", LINE, LINE);
    print_list(head);
    printf("%s%s%s\n", LINE, LINE, LINE);
    // ------------------------------------------
    
    get_query(atoi(argv[2]));
    
    free_list(head); // need to do an exit function
    
    return 0;
}