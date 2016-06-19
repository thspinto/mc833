#ifndef CMESSENGER_SERVER_H
#define CMESSENGER_SERVER_H


#include "Client.h"
#include "Message.h"
#include "Group.h"
#include <list>
#include <map>
#include <queue>
#include <string>
#include <sstream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

#define LISTENQ 10
#define MAXLINE 1024

class Server {
    std::list<Group> groupList;
    std::list<Client> clientList;
    std::queue<Message> messageQueue;
    std::map<int, Message>incompleteMessageMap;
    std::map<int, Client*> connectedClientMap;
    int port;

    /*
     * Pega o cliente associado ao file descriptor do socket. Se não houver um, a função retorna um novo cliente com
     * apenas o campo socketfd setado.
     *
     * @param socketfd: file descripor do socket
     * @return cliente connectado àquele socket
     */
    Client getClient(int socketfd);

    /*
     * Chama a função adequada para tratar o comando recebido do cliente.
     *
     * @param command: o comando a ser executado
     * @param message: a mensagem que o comando tratará
     */
    void executeCommand(Message::Action command, Message& message);

    /*
     * Imprime os endereços de IP locais
     */
    void printLocalAddress();

    /*
     * Faz o bind e o listen.
     */
    int startListen();

    /*
     * Recebe e trata informações do socket.
     */
    void selectLoop(int listenfd);

public:
    /*
     * Inicia o servidor
     *
     * @param port: Porta que irá escutar por novos clientes
     */
    int run(int port);
};


#endif //CMESSENGER_SERVER_H
