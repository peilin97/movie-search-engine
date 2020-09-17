/**
 * Modified by Peilin Guo
 * Apr 15, 2020
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

#include "QueryProtocol.h"
#include "MovieSet.h"
#include "MovieIndex.h"
#include "DocIdMap.h"
#include "Hashtable.h"
#include "QueryProcessor.h"
#include "FileParser.h"
#include "DirectoryParser.h"
#include "FileCrawler.h"
#include "Util.h"

#define BUFFER_SIZE 1000

extern int errno;

int Cleanup();

DocIdMap docs;
MovieTitleIndex docIndex;

#define SEARCH_RESULT_LENGTH 1500

char movieSearchResult[SEARCH_RESULT_LENGTH];
int sock_fd;
struct addrinfo *result;

void sigchld_handler(int s) {
    write(0, "Handling zombies...\n", 20);
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


void sigint_handler(int sig) {
    write(0, "Ahhh! SIGINT!\n", 14);
    Cleanup();
    exit(0);
}

/**
 * Return 0 for successful connection;
 * Return -1 for some error.
 */
int HandleClient(int client_fd, char *query) {
    printf("Getting docs for movieset term: \"%s\"\n", query);
    // Run query and get responses
    DocumentSet documentSet = GetDocumentSet(docIndex, query);
    int num;
    char buffer[BUFFER_SIZE];
    if (documentSet == NULL) {
        num = 0;
        printf("num_responses: %d\n", num);
        send(client_fd, &num, sizeof(num), 0);
        int len = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        buffer[len] = '\0';
        if (CheckAck(buffer) != 0) {
            printf("I expected an ACK. Instead received: %s\n", buffer);
            printf("NO ACK. Breaking...\n");
            exit(0);
        }
    } else {
        // handle non-empty searching result
        // Send number of responses
        num = NumMoviesInSet(documentSet);
        printf("num_responses: %d\n", num);
        send(client_fd, &num, sizeof(num), 0);
        int len = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        buffer[len] = '\0';
        if (CheckAck(buffer) != 0) {
            printf("I expected an ACK. Instead received: %s\n", buffer);
            printf("NO ACK. Breaking...\n");
            exit(0);
        }
        SearchResultIter iter = CreateSearchResultIter(documentSet);
        SearchResult movie;
        movie = (SearchResult) malloc(sizeof(struct searchResult));
        movie->doc_id = (uint64_t) 0;
        movie->row_id = 0;

        SearchResultGet(iter, movie);
        char movieSearchResult[SEARCH_RESULT_LENGTH];
        CopyRowFromFile(movie, docs, movieSearchResult);
        send(client_fd, movieSearchResult, strlen(movieSearchResult), 0);
        // Wait for ACK
        len = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        buffer[len] = '\0';
        if (CheckAck(buffer) != 0) {
            printf("I expected an ACK. Instead received: %s\n", buffer);
            printf("NO ACK. Breaking...\n");
            // close(client_fd);
            DestroySearchResultIter(iter);
            free(movie);
            exit(0);
        }
        // For each response
        while (SearchResultIterHasMore(iter)) {
            SearchResultNext(iter);
            SearchResultGet(iter, movie);
            CopyRowFromFile(movie, docs, movieSearchResult);
            // Send response
            send(client_fd, movieSearchResult, strlen(movieSearchResult), 0);
            // Wait for ACK
            len = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
            buffer[len] = '\0';
            if (CheckAck(buffer) != 0) {
                printf("I expected an ACK. Instead received: %s\n", buffer);
                printf("NO ACK. Breaking...\n");
                // close(client_fd);
                DestroySearchResultIter(iter);
                free(movie);
                exit(0);
            }
        }
        // Cleanup
        free(movie);
        DestroySearchResultIter(iter);
    }
    // close connection.
    // Send GOODBYE
    SendGoodbye(client_fd);
    printf("exiting child process\n");
    exit(0);
}

int HandleConnections(int sock_fd, int debug) {
    // int child_status;
    // Step 5: Accept connection
    printf("Waiting for connection...\n");
    int client_fd = accept(sock_fd, NULL, NULL);
    int errnum = errno;
    printf("Connection made: client_fd = %d\n", client_fd);
    fprintf(stderr, "Socket Error: %s\n", strerror(errnum));
    // Fork on every connection
    if (fork() == 0) {
        close(sock_fd);
        if (debug == 1) {
            sleep(10);
        }
        // Child handles new connection
        char query[BUFFER_SIZE];
        // Send ACK
        SendAck(client_fd);
        // Listen for query
        int len = recv(client_fd, query, BUFFER_SIZE - 1, 0);
        query[len] = '\0';
        printf("checking goodbye...%s\n", query);
        // If query is GOODBYE close connection
        if (CheckGoodbye(query) == 0) {
            close(client_fd);
            exit(0);
        } else {
            HandleClient(client_fd, query);
            close(client_fd);
            exit(0);
        }
    }
    close(client_fd);
    return 0;
}

int Setup(char *dir) {
    struct sigaction sa;

    sa.sa_handler = sigchld_handler;  // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    struct sigaction kill;

    kill.sa_handler = sigint_handler;
    kill.sa_flags = 0;  // or SA_RESTART
    sigemptyset(&kill.sa_mask);

    if (sigaction(SIGINT, &kill, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("Crawling directory tree starting at: %s\n", dir);
    // Create a DocIdMap
    docs = CreateDocIdMap();
    CrawlFilesToMap(dir, docs);
    printf("Crawled %d files.\n", NumElemsInHashtable(docs));

    // Create the index
    docIndex = CreateMovieTitleIndex();

    if (NumDocsInMap(docs) < 1) {
        printf("No documents found.\n");
        return 0;
    }

    // Index the files
    printf("Parsing and indexing files...\n");
    ParseTheFiles(docs, docIndex);
    printf("%d entries in the index.\n", NumElemsInHashtable(docIndex->ht));
    return NumElemsInHashtable(docIndex->ht);
}

int Cleanup() {
    close(sock_fd);
    freeaddrinfo(result);
    DestroyMovieTitleIndex(docIndex);
    DestroyDocIdMap(docs);
    return 0;
}

int main(int argc, char **argv) {
    // port
    char *port = NULL;
    char *dir_to_crawl = NULL;

    int debug_flag = 0;
    int c;

    while ((c = getopt(argc, argv, "dp:f:")) != -1) {
        switch (c) {
            case 'd':
                debug_flag = 1;
                break;
            case 'p':
                port = optarg;
                break;
            case 'f':
                dir_to_crawl = optarg;
                break;
        }
    }

    if (port == NULL) {
        printf("No port provided; please include with a -p flag.\n");
        exit(0);
    }

    if (dir_to_crawl == NULL) {
        printf("No directory provided; please include with a -f flag.\n");
        exit(0);
    }

    int num_entries = Setup(dir_to_crawl);
    if (num_entries == 0) {
        printf("No entries in index. Quitting. \n");
        exit(0);
    }

    int s;

    // Step 1: Get address stuff
    struct addrinfo hints;
    //  Setting up the hints struct...
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    s = getaddrinfo("localhost", port, &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(1);
    }
    // Step 2: Open socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    // Step 3: Bind socket
    if (bind(sock_fd, result->ai_addr, result->ai_addrlen) != 0) {
        perror("bind()");
        exit(1);
    }
    // Step 4: Listen on the socket
    if (listen(sock_fd, 10) != 0) {
        perror("listen()");
        exit(1);
    }
    int i = 0;
    while (i < 20) {
        HandleConnections(sock_fd, debug_flag);
        i++;
    }
    // Got Kill signal
}
