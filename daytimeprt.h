#ifndef NETWORKPROGRAMMING_DAYTIMEPRT_H
#define NETWORKPROGRAMMING_DAYTIMEPRT_H
void connectToHostname(char *hostname, int *connection_socket, char *port_number);
int startDayTimePrtServer(int argc, char **argv);
int startDayTimePrtClient(int argc, char **argv);
#endif //NETWORKPROGRAMMING_DAYTIMEPRT_H