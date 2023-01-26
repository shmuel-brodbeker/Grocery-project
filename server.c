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
#define LINE "============="

List *head = NULL; // global head

void send_for_processing (char *buffer, int max_len)
{
    char command[30] = {0};
    sscanf(buffer, "%s", command);

    if (!strcmp(command, "select"))
    {
        // Select *pro_query = check_select_query (buffer + strlen(command) + 1);
        // if (pro_query)
        // {
        //     int counter = 0;
        //     print_query (pro_query, head, &counter);
        //     if (counter == 0)
        //         puts("No data found to display");
        // }
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
        print_list(head); // --------------------------------
        snprintf(buffer, MAX_LEN, "Print list:\n");
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

    // ----------------------------------
        send_for_processing(buffer, MAX_LEN); 
    // ----------------------------------

    // printf("Server received: %s\n", buffer); // ---- debug ----

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
while (1)
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
    // while (1)
    for (int i = 0; i < 5; i++)
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
    
    free_list(head);
    
    return 0;
}