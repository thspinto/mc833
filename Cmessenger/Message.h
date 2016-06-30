#ifndef CMESSENGER_MESSAGE_H
#define CMESSENGER_MESSAGE_H

#include <string>
#include <list>
#include <sstream>
#include <vector>
#include <map>
#include <sys/socket.h>
#include "User.h"

class Message {

public:
    std::string id;
    int groupMessageId;
    std::string originalMessageHash;
    std::string groupHeader;
    const User *origin;
    const User *dest;
    int size;
    int expectedSize;
    std::vector<char> buf;
    enum Action {
        UNkNOWN,
        CONN,
        SEND,
        CREATEG,
        JOING,
        SENDG,
        WHO,
        SENDF,
        GETF,
        EXIT
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

    /*
     * Pega o conteúdo da mensagem.
     *
     * @return: conteúdo da mensagem em formato string
     */
    std::string toString();

    Message(const User* origin, const User* dest, std::vector<char> buf): origin(origin), dest(dest), buf(buf),
                                                                           size(buf.size()) {};

    Message() {};
};


#endif //CMESSENGER_MESSAGE_H
