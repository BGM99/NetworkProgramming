#include <strings.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "chat.h"
#include <stdlib.h>

#define BUFFER 50
#define BACKLOG 10
#define MAX_CLIENTS 10


void printSockAddrIn(const struct sockaddr_in *sockAddr) {
    char ipAddr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sockAddr->sin_addr), ipAddr, INET_ADDRSTRLEN);

    printf("IP: %s\n", ipAddr);
    printf("Port: %d\n", ntohs(sockAddr->sin_port));
}


int startChatClient(int argc, char *argv[]) {

    int connfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;

    if (inet_pton(AF_INET, argv[2], &server.sin_addr) <= 0) {
        fprintf(stderr, "Invalid IP address: %s\n", argv[2]);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[3]));

    printSockAddrIn(&server);

    if (connect(connfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
        printf("Connection to given IP address failed. \n");
    }

    int n;
    int pid = fork();

    while (pid == 0) { //child
        char buff[BUFFER];
        //read and print MSG from Server
        if ((n = read(connfd, buff, BUFFER)) > 0) {
            buff[n] = 0;        /* null terminate */
            printf("%s\n", buff);
        }
    }

    while (pid != 0) { //parent
        int count = 0;
        char buff[BUFFER];

        while (buff[count - 1] != EOF && buff[count - 1] != '\n') {
            buff[count] = getchar();
            count++;
        }

        write(connfd, buff, count);
    }

    close(connfd);
}

void *serveClient(void *arg) {
    struct ThreadParams *params = (struct ThreadParams *) arg;
    int *openConnections = params->array;
    size_t size = params->size;
    int clientFd = params->clientFD;

    int n;

    while (1) {
        char buff[BUFFER];

        if ((n = read(clientFd, buff, BUFFER)) > 0) {
            for (int i = 0; i < size; ++i) {
                int conn = openConnections[i];
                if (conn == 0 || conn == clientFd) {
                    continue;
                }
                if (write(conn, buff, n) < 0) {
                    openConnections[i] = 0;
                    printf("Connection to client lost!");
                }
            }

        }
    }

    pthread_exit(NULL);
}

int startChatServer(int argc, char *argv[]) {

    int openConnection[MAX_CLIENTS];
    bzero(openConnection, MAX_CLIENTS);

    struct sockaddr_in client;

    bzero(&client, sizeof client);
    client.sin_addr.s_addr = INADDR_ANY;
    client.sin_port = htons(1871);
    client.sin_family = AF_INET;

    int listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(listenfd, (struct sockaddr *) &client, sizeof client) < 0) {
        printf("Binding address to socket failed");
    }

    if (listen(listenfd, BACKLOG) < 0) {
        printf("Changing the socket to listen mode failed!");
    }

    while (1) {
        int clientfd = accept(listenfd, 0, 0);
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            int conn = openConnection[i];
            if (conn == 0) {
                openConnection[i] = clientfd;
                break;
            }
        }

        struct ThreadParams params = {
                .array = openConnection,
                .size = MAX_CLIENTS,
                .clientFD = clientfd
        };

        pthread_t listenThread;
        pthread_create(&listenThread, NULL, serveClient, (void *) &params);
    }
}