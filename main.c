#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include "daytimeprt.h"
#include "webserver.h"
#include "echoprt.h"

int printHostIp(const char *hostName);

int main(int argc, char *argv[]) {
    int err;

    //err = printHostIp("localhost");

    //if (!strcmp(argv[1], "server")){
    //    err = startDayTimePrtServer(argc, argv);
    //} else {
    //    err = startDayTimePrtClient(argc, argv);
    //}

    err = startHttpGetServer(argc, argv);

    return err;
}

int printHostIp(const char *hostName) {
    int err;
    struct sockaddr_in* sa;
    struct addrinfo* res = NULL;

    err = getaddrinfo(hostName, "80", 0, &res);
    sa = (struct sockaddr_in *) res->ai_addr;

    char str[INET_ADDRSTRLEN];
    printf("%s\n", inet_ntop(AF_INET, &sa->sin_addr, str, sizeof(str)));

    return err;
}