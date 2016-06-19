#ifndef CMESSENGER_MESSAGE_H
#define CMESSENGER_MESSAGE_H

#include <string>
#include <list>
#include <sstream>
#include <vector>
#include "Client.h"

class Message {

public:
    int id;
    Client *origin;
    Client *dest;
    int size;
    int expectedSize;
    std::vector<char> buf;
    enum Action {
        CONN,
        SEND,
        CREATEG,
        JOING,
        SENDG,
        WHO,
        EXIT
    };

    /*
    * Parseia a mensagem recebida. Remove o comando no início da mensagem e o retorna como enum.
    *
    * @param message: a mensagem completa recebida do cliente
    *
    */
    Action parse();
};


#endif //CMESSENGER_MESSAGE_H
