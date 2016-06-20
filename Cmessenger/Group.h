#ifndef CMESSENGER_GROUP_H
#define CMESSENGER_GROUP_H

#include <string>
#include <set>

class Client;
class Group {

public:
    std::string name;
    std::set<Client*> clients;

    Group(std::string name): name(name){};
    Group() {};
};


#endif //CMESSENGER_GROUP_H
