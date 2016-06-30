#ifndef CMESSENGER_CLIENT_H
#define CMESSENGER_CLIENT_H

#include <string>
#include <sstream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "md5.h"

#define MAX_LINE 1024

class Client {

public:
    int id;
    int socketfd;
    int serverPort;
    char *host;
    char* user;

    bool createConnection();
    int run();
    int getAndSendMessages();
    void sendMessage(std::string message);
    void recvMessage();
    void connection();

    Client(char *lh, int port, char *user): host(lh), serverPort(port), user(user) {}
    Client() {}
};


#endif //CMESSENGER_CLIENT_H
