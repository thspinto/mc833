#ifndef CMESSENGER_GROUP_H
#define CMESSENGER_GROUP_H

#include <string>
#include <set>
#include <vector>

class User;
class Group {

public:
    std::string name;
    std::set<User*> clients;
    std::vector<int> messageCount;

    Group(std::string name): name(name){};
    Group() {};
};


#endif //CMESSENGER_GROUP_H
