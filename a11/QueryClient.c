/*
 * Modified by Peilin Guo
 * Apr 13, 2020
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "QueryProtocol.h"

char *port_string = "1500";
unsigned short int port;
char *ip = "127.0.0.1";

#define BUFFER_SIZE 1000
#define QUERY_SIZE 100

void RunQuery(char *query) {
    // Find the address
    // Create the socket
    // Connect to the server
    // Do the query-protocol
    // Close the connection
    char resp[BUFFER_SIZE];
    struct addrinfo hints, *result;
    hints.ai_family = AF_INET; /* IPv4 only */
    hints.ai_socktype = SOCK_STREAM; /* TCP */
    memset(&hints, 0, sizeof(struct addrinfo));

    getaddrinfo(ip, port_string, &hints, &result);
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock_fd, result->ai_addr, result->ai_addrlen);
    printf("Connected to the server.\n\n");
    recv(sock_fd, resp, BUFFER_SIZE - 1, 0);  // ACK
    send(sock_fd, query, strlen(query), 0);  // send the query

    // get the size
    int size;
    recv(sock_fd, &size, BUFFER_SIZE - 1, 0);
    SendAck(sock_fd);
    printf("size=%d\n", size);

    // get the movie and print
    for (int i = 0; i < size; i++) {
        int len = recv(sock_fd, resp, BUFFER_SIZE - 1, 0);
        SendAck(sock_fd);
        resp[len] = '\0';
        printf("%s\n", resp);
    }
    recv(sock_fd, resp, BUFFER_SIZE - 1, 0);  // GOODBYE
    freeaddrinfo(result);
    close(sock_fd);
}

void RunPrompt() {
    char input[BUFFER_SIZE];

    while (1) {
        printf("Enter a term to search for, or q to quit: ");
        scanf("%s", input);

        printf("input was: %s\n", input);

        if (strlen(input) == 1) {
            if (input[0] == 'q') {
                printf("Thanks for playing! \n");
                return;
            }
        }
        // Limit query from user to 100 characters
        if (strlen(input) > QUERY_SIZE) {
            printf("the length of the term should not exceed 100 characters.\n");
        } else {
            printf("\n\n");
            RunQuery(input);
        }
    }
}

// This function connects to the given IP/port to ensure
// that it is up and running, before accepting queries from users.
// Returns 0 if can't connect; 1 if can.
int CheckIpAddress(char *ip, char *port) {
    struct addrinfo hints, *result;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    memset(&hints, 0, sizeof(struct addrinfo));
    int s = getaddrinfo(ip, port, &hints, &result);
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (s) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return 0;
    }
    // Connect to the server
    if (connect(sock_fd, result->ai_addr, result->ai_addrlen) == -1) {
        perror("connect");
        return 0;
    }
    // Listen for an ACK
    char resp[BUFFER_SIZE];
    int len = recv(sock_fd, resp, BUFFER_SIZE - 1, 0);
    resp[len] = '\0';
    if (CheckAck(resp) != 0) {
        printf("expect receiving ACK, but receive %s\n", resp);
        return 0;
    }
    // Send a goodbye
    SendGoodbye(sock_fd);
    // Close the connection
    freeaddrinfo(result);
    close(sock_fd);
    return 1;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Incorrect number of arguments. \n");
        printf("Correct usage: ./queryclient [IP] [port]\n");
    } else {
        ip = argv[1];
        port_string = argv[2];
    }

    if (CheckIpAddress(ip, port_string)) {
        RunPrompt();
    }
    return 0;
}
