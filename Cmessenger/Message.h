#ifndef CMESSENGER_MESSAGE_H
#define CMESSENGER_MESSAGE_H

#include <string>
#include <list>
#include <sstream>
#include <vector>
#include <map>
#include <sys/socket.h>
#include "Client.h"

class Message {

public:
    int id;
    const Client *origin;
    const Client *dest;
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
    };

    static std::map<std::string, Action> actionMap;

    /*
    * Parseia a mensagem recebida. Remove o comando no início da mensagem e o retorna como enum.
    *
    * @return: ação a ser executada com a mensagem
    */
    Message::Action parseAction();

    /*
     * Pega o próximo parâmentro do comando.
     *
     * @return: o próximo parâmetro do comando
     */
    std::string parseCommandParameter();


    /*
     * Seta o buffer da mensagem.
     *
     * @param buffer: buffer da mensagem
     * @param size: tamanho do buffer
     */
    void setBuffer(const char* buffer, int size);

    /*
     * Envia mensagem para destino.
     *
     * @return true se mensagem for enviada.
     */
    bool sendMessage();

    Message(const Client* origin, const Client* dest, std::vector<char> buf): origin(origin), dest(dest), buf(buf),
                                                                           size(buf.size()) {};

    Message() {};
};


#endif //CMESSENGER_MESSAGE_H
