//
// Created by Thiago Pinto on 5/27/16.
//

#ifndef CMESSENGER_MESSAGE_H
#define CMESSENGER_MESSAGE_H

#include <string>
#include <list>
#include "Client.h"

class Message {

public:
    int id;
    Client& origin;
    Client& dest;
    std::string message;

};


#endif //CMESSENGER_MESSAGE_H
