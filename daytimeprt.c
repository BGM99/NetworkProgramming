#include "daytimeprt.h"
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>

#define MAXLINE 4096
#define BACKLOG 10

void connectToHostname(char *hostname, int *connection_socket, char *port_number) {
    int check_sfd;
    struct addrinfo hints, *p, *servinfo;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;
    hints.ai_protocol = 0;

    int res = getaddrinfo(hostname, port_number, &hints, &servinfo);
    if (res < 0) {
        fprintf(stderr, "Error looking up %s", hostname);
        exit(1);
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        check_sfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (check_sfd == -1)
            continue;

        if (connect(check_sfd, p->ai_addr, p->ai_addrlen) == 0) {
            *connection_socket = check_sfd;
            freeaddrinfo(servinfo);
            printf("Successfully connected to hostname.\n");
            return;
        }
        close(check_sfd);
    }

    fprintf(stderr, "Couldn't connect to %s", hostname);
}

int startDayTimePrtClient(int argc, char **argv) {
    int fd;
    int n;
    char buff[MAXLINE];

    if (argc != 2) {
        printf("usage: %s <host-IP>\n", argv[0]);
        return -1;
    }

    connectToHostname(argv[1], &fd, "1111");

    while ((n = read(fd, buff, MAXLINE)) > 0) {
        buff[n] = 0;        /* null terminate */
        printf("%s\n", buff);
    }
    close(fd);

    return 1;
}

int startDayTimePrtServer(int argc, char **argv) {
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    char buff[MAXLINE];
    time_t ticks;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(1111);	/* daytime server */

    bind(listenfd, (const struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listenfd, BACKLOG);

    while (1) {
        connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        write(connfd, buff, strlen(buff));

        close(connfd);
    }
}