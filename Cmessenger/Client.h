#ifndef CMESSENGER_CLIENT_H
#define CMESSENGER_CLIENT_H

#include <string>
#include <list>
#include "Message.h"
#include "Group.h"
#include <list>
#include <map>
#include <queue>
#include <string>
#include <sstream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#define MAX_LINE 256

class Group;
class Client {

public:
    int socketfd;
    int id;
    std::string user;
    std::list<Group*> groups;
    Client(std::string user, int socketfd): user(user), socketfd(socketfd) {}
    Client() {}
    int serverPort;
    char *host;

    bool createConnection();

    int run(char *lh, int port, char *user);

    int getAndsendMessages();
};


#endif //CMESSENGER_CLIENT_H
