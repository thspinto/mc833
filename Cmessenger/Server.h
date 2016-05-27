//
// Created by Thiago Pinto on 5/27/16.
//

#ifndef CMESSENGER_SERVER_H
#define CMESSENGER_SERVER_H


#include "Client.h"
#include "Message.h"
#include "Group.h"
#include <list>
#include <map>
#include <queue>
#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#define LISTENQ 5

class Server {
    std::list<Group> groupList;
    std::list<Client> clientList;
    std::queue<Message> messageQueue;
    std::map<int, Client> connectedClientMap;
    enum ServerAction {
        PENDING_INPUT,
        PARCE_REQ,
        CONN,
        SEND,
        CREATEG,
        JOING,
        SENDG,
        WHO,
        EXIT
    };

    /*
    * Parseia a mensagem recebida. Remove o comando no início da mensgem e o retorna como enum.
    *
    * @param message: a mensagem completa recebida do cliente
    * @return o comando a ser executado com a mensagem
    */
    ServerAction parseMessage(std::string message);

    /*
     * Pega o cliente associado ao file descriptor do socket. Se não houver um, a função retorna um novo cliente com
     * apenas o campo socketFD setado.
     *
     * @param socketFD: file descripor do socket
     * @return cliente connectado àquele socket
     */
    Client getClient(int socketFD);

    /*
     * Chama a função adequada para tratar o comando recebido do cliente.
     *
     * @param command: o comando a ser executado
     * @param message: a mensagem que o comando tratará
     * @param client: o cliente que fez a requisição
     */
    void executeCommand(ServerAction command, Client client, std::string message);

    /*
     * Imprime os endereços de IP locais
     *
     * @return 0 se sucesso
     */
    int printLocalAddress(int port);

public:
    /*
     * Inicia o servidor
     *
     * @param port: Porta que irá escutar por novos clientes
     */
    int run(int port);
};


#endif //CMESSENGER_SERVER_H
