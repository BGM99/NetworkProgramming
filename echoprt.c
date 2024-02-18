#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "echoprt.h"

#define MAXLINE 4096
#define BACKLOG 10

int startEchoPrtServer(int argc, char **argv) {
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    char buff[MAXLINE];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(2222);

    bind(listenfd, (const struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listenfd, BACKLOG);

    connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);

    while (1) {
        long offset = 0;
        do {
            offset += read(connfd, buff + offset, 1);
        } while (buff[offset - 1] != '\n');

        write(connfd, buff, strlen(buff));
        bzero(buff, offset);
    }
}

int startEchoMultiProcessServer(int argc, char **argv) {
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    char buff[MAXLINE];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(2222);

    bind(listenfd, (const struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listenfd, BACKLOG);

    int pid = getpid();
    while (pid != 0) {
        connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);
        pid = fork();
    }

    while (1) {
        long offset = 0;
        do {
            offset += read(connfd, buff + offset, 1);
        } while (buff[offset - 1] != '\n');

        write(connfd, buff, strlen(buff));
        bzero(buff, offset);
    }
}