//
// Created by Thiago Pinto on 5/27/16.
//

#ifndef CMESSENGER_GROUP_H
#define CMESSENGER_GROUP_H

#include <string>
#include <list>

class Client;
class Group {

public:
    std::string name;
    std::list<Client*> clients;

};


#endif //CMESSENGER_GROUP_H
