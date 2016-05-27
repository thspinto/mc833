//
// Created by Thiago Pinto on 5/27/16.
//

#ifndef CMESSENGER_CLIENT_H
#define CMESSENGER_CLIENT_H

#include <string>
#include <list>

class Group;
class Client {

public:
    int soketFD;
    int id;
    std::string user;
    std::list<Group*> groups;

};


#endif //CMESSENGER_CLIENT_H
