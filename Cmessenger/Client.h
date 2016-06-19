#ifndef CMESSENGER_CLIENT_H
#define CMESSENGER_CLIENT_H

#include <string>
#include <list>

class Group;
class Client {

public:
    int socketfd;
    int id;
    std::string user;
    std::list<Group*> groups;
    Client(std::string user, int socketfd): user(user), socketfd(socketfd) {}
    Client() {}
};


#endif //CMESSENGER_CLIENT_H
