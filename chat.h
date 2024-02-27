#ifndef NETWORKPROGRAMMING_CHAT_H
#define NETWORKPROGRAMMING_CHAT_H
int startChatClient(int argc, char *argv[]);
struct ThreadParams {
    int *array;
    size_t size;
    int clientFD;
};
int startChatServer(int argc, char *argv[]);
void *serveClient(void *arg);
#endif //NETWORKPROGRAMMING_CHAT_H
