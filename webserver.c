#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "webserver.h"

#define MAXLINE 4096
#define BUFFSIZE 10
#define BACKLOG 10

int startHttpGetServer(int argc, char **argv) {
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

    if (connfd > 0) {
        long offset = 0;
        do {
            offset += read(connfd, buff + offset, 1);
        } while (buff[offset - 1] != '\n');

        buff[offset - 2] = 0x00; //set string termination

        int fd, n;
        fd = open(buff, O_RDONLY);

        if (fd < 0) {
            char* error = "The File does not exist / could not be opened!\n";
            write(connfd, error, strlen(error));
        } else {
            bzero(buff, offset);
            while ((n = read(fd, buff, BUFFSIZE)) > 0){
                write(connfd, buff, n);
            }
            write(connfd, "\n", 1);
        }

        close(fd);
        close(connfd);
        int err;
        waitpid(pid, &err, 0); //clean up child process
    }
}