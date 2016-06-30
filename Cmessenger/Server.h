#ifndef CMESSENGER_SERVER_H
#define CMESSENGER_SERVER_H


#include "User.h"
#include "Message.h"
#include "Group.h"
#include "md5.h"
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
    int maxfd, sockfd, listenfd;
    fd_set allset;
    std::map<std::string, Group> groupMap;
    std::map<std::string, User> clientMap;
    std::list<Message> messageQueue;
    std::map<int, Message> incompleteMessageMap;
    std::map<int, User *> connectedClientMap;
    int port;

    /*
     * Pega o cliente associado ao file descriptor do socket. Se não houver um, a função retorna um novo cliente com
     * apenas o campo socketfd setado.
     *
     * @param socketfd: file descripor do socket
     * @return cliente connectado àquele socket
     */
    User getClient(int socketfd);

    /*
     * Chama a função adequada para tratar o comando recebido do cliente.
     *
     * @param command: o comando a ser executado
     * @param message: a mensagem que o comando tratará
     */
    void executeCommand(Message::Action command, Message &message);

    /*
     * Imprime os endereços de IP locais
     */
    void printLocalAddress();

    /*
     * Faz o bind e o listen.
     */
    void startListen();

    /*
     * Recebe e trata informações do socket.
     */
    void selectLoop();

    /*
     * Conexão de novo cliente. Cria o cliente.
     *
     * @param message: mensagem recebida
     */
    void conn(Message& message);

    /*
     * Popula os clientes de origem, destino, e envia a mensagem.
     *
     * @param message: mensagem recebida
     */
    void send(Message& message);

    /*
     * Verifica se o cliente no socket atual foi identificado.
     *
     * @return true se socket estiver associado a um usuário
     */
    bool verifyConnectedClient();

    /*
     * Pega o cliente de destino. Se não existir envia mensagem de erro para cliente de origem.
     *
     * @param destUser: nome do usuário de destino.
     * @return o Cliente se existir, NULL caso contrário.
     */
    User* verifyDestClient(std::string destUser);

    /*
     * Cria um novo grupo
     *
     * @param message: mensagem recebida
     */
    void createg(Message& message);

    /*
     * Adiciona usuário ao grupo
     *
     * @param message: mensagem recebida
     */
    void joing(Message& message);

    /*
     * Envia mensagem ao grupo
     *
     * @param message: mensagem recebida
     */
    void sendg(Message& message);

    /*
     * Envia lista de usuários
     */
    void who();

    /*
     * Requisição de envio de arquivo para outro usuário
     */
    void sendf(Message& message);

    /*
     * Entrega aquivo ao cliente
     */
    void getf(Message& message);

    /*
     * Fecha o socket e remove do mapa de clientes conectados.
     */
    void closeSocket(int sockfd);

    /*
     * Calcula o id da mensagem
     *
     * @param menssage: a mensagem
     * @return: id da mensagem
     */
    std::string getMessageId(Message& message);

public:
    /*
     * Inicia o servidor.
     *
     * @param port: Porta que irá escutar por novos clientes
     */
    int run(int port);

    void sendServerMessage(std::string &status) const;

    /*
     * Envia mensagem de confimação de entrega.
     *
     * @param message: mensagem entregue
     * @return a mesagem caso não seja entregue imediatamente
     */
    void sendDeliveryNotification(Message &message);
};


#endif //CMESSENGER_SERVER_H
