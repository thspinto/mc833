#ifndef CMESSENGER_CLIENT_H
#define CMESSENGER_CLIENT_H

#include <string>
#include <list>

#define MAX_LINE 256

class Group;
class User {

public:
    int socketfd;
    int id;
    std::string user;
    std::list<Group*> groups;
    User(std::string user, int socketfd): user(user), socketfd(socketfd) {}
    User() {}
};


#endif //CMESSENGER_CLIENT_H
