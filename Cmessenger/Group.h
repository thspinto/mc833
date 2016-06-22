#ifndef CMESSENGER_GROUP_H
#define CMESSENGER_GROUP_H

#include <string>
#include <set>
#include <map>

class Client;
class Group {

public:
    std::string name;
    std::set<Client*> clients;
    std::map<std::string, int> messageCount;

    Group(std::string name): name(name){};
    Group() {};
};


#endif //CMESSENGER_GROUP_H
