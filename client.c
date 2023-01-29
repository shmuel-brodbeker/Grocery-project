#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_LEN 1024

int main(int argc, char **argv)
{
    int sockfd[5];
    struct sockaddr_in server_addr;
    char buffer[5][MAX_LEN];
    int n;
    int i, j;

    if (argc < 2)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    /* Connect to the server */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(atoi(argv[1]));

    /* Send data to the server */
    while (1)
    {
        /* Create a sockets */
        for (i = 0; i < 5; i++)
        {
            sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd[i] < 0)
            {
                perror("Error creating socket");
                return 1;
            }
            if (connect(sockfd[i], (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
            {
                perror("Error connecting");
                return 1;
            }
        }

        memset(buffer, 0, sizeof(buffer));
        puts("\nEnter five commands. Begin with: select, set, print, quit\n");
        
        for (i = 0; i < 5; i++)
        {
            fgets(buffer[i], MAX_LEN, stdin);
            if (!strncmp(buffer[i], "quit", 4))
            {
                puts ("Exiting... have a good day");
                for (int j = 0; j < 5; j++)
                {
                    close(sockfd[j]); // dose i need to close befor exiting
                }
                return 0;
            }
            if (!strncmp(buffer[i], "send", 4))
            {
                break;
            }
            if (strlen(buffer[i]) < 2)
            {
                i--;
                continue;
            }
        }

        for (j = 0; j < i; j++)
        {
            n = send(sockfd[j], buffer[j], strlen(buffer[j]) - 1, 0);
            if (n < 0)
            {
                perror("Client error sending data");
                return 1;
            }
        }

    /* ---------- Waiting for reply ---------- */

        puts("\n========= server sending ==========");
        for (j = 0; j < i; j++)
        {
            printf(" Query %d:\n", j+1);
            do {
                n = recv(sockfd[j], buffer[j], MAX_LEN, 0);
                if (n < 0)
                {
                    perror("Client error receiving data");
                    break; // return 1 ?
                }
                buffer[j][n] = '\0'; 
                printf("%s\n", buffer[j]);
            } while (n > 0);
        }

        for (i = 0; i < 5; i++)
        {
            close(sockfd[i]);
        }
    }

    return 0;
}