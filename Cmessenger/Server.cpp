#include "Server.h"
#include "easylogging++.h"

int Server::run(int port) {
    Server::port = port;
    startListen();
    selectLoop();

    return 0;
}

void Server::startListen() {
    struct sockaddr_in servaddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(Server::port);

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)  {
        perror("bind error");
        close(listenfd);
        exit(1);
    }

    if (listen(listenfd, LISTENQ) < 0) {
        perror("listen error");
        close(listenfd);
        exit(1);
    }
    Server::printLocalAddress();
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void Server::selectLoop() {
    int connfd, nready, n;
    fd_set rset;
    socklen_t clilen;
    struct sockaddr_in	cliaddr;
    std::vector<char> buf(MAXLINE);

    maxfd = listenfd;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    while(1){
        rset = allset;
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);

        // Verifica se novo cliente conectou
        if (FD_ISSET(listenfd, &rset)) {
            clilen = sizeof cliaddr;
            connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
            connectedClientMap[connfd] = NULL;
            FD_SET(connfd, &allset);
            if (connfd > maxfd) {
                maxfd = connfd;
            }

            LOG(INFO) << "Client Connected: " << inet_ntoa(cliaddr.sin_addr) << ":" << cliaddr.sin_port;

            if (--nready <= 0) {
                continue;
            }
        }

        //Recebe mensagens dos clientes conectados
        for(std::map<int, Client*>::iterator it = connectedClientMap.begin();
            it != connectedClientMap.end(); it++) {
            if(connectedClientMap.size() < 1){
                continue;
            }

            sockfd = it->first;
            if (FD_ISSET(sockfd, &rset)) {
                if ((n = read(sockfd, buf.data(), buf.size())) == 0) {
                    //Cliente desconectou
                    Server::closeSocket(sockfd);
                } else {
                    Message message;
                    if(incompleteMessageMap.find(sockfd) != incompleteMessageMap.end()){
                        message = incompleteMessageMap[sockfd];
                        message.expectedSize -= n;
                    } else {
                        int size;
                        std::stringstream ss(buf.data());
                        ss >> size;
                        message.size = n;
                        message.expectedSize = size - n + ((int)ss.tellg() + 1);
                    }

                    message.buf.insert(message.buf.end(), buf.begin(), buf.end());

                    if(message.expectedSize > 0) {
                        incompleteMessageMap[sockfd] = message;
                    } else {
                        incompleteMessageMap.erase(sockfd);
                        executeCommand(message.parseAction(), message);
                    }
                }
                if (--nready <= 0)
                    break; //sem clientes conectados
            }
        }
    }
}

void Server::closeSocket(int sockfd) {
    socklen_t clilen;
    struct sockaddr_in	cliaddr;

    clilen = sizeof cliaddr;
    getpeername(sockfd, (struct sockaddr*)&cliaddr, &clilen);

    LOG(INFO) << "Client disconnected: " << inet_ntoa(cliaddr.sin_addr) << ":" << cliaddr.sin_port;

    close(sockfd);
    FD_CLR(sockfd, &allset);

    Client* client = connectedClientMap.find(sockfd)->second;
    connectedClientMap.erase(sockfd);

    if(client != NULL) {
        client->socketfd = -1;
    }

    if(connectedClientMap.size() > 0){
        maxfd = (--connectedClientMap.end())->first;
    } else {
        maxfd = listenfd;
    }
}

#pragma clang diagnostic pop

void Server::executeCommand(Message::Action command, Message &message) {
    switch (command) {
        case Message::CONN :
            Server::conn(message);
            break;
        case Message::SEND :
            break;
        case Message::CREATEG :
            break;
        case Message::JOING :
            break;
        case Message::SENDG :
            break;
        case Message::WHO :
            break;
    }
}

void Server::conn(Message &message) {
    std::string user(message.parseCommandParameter());
    std::string status = "CONN Usuário conectado\n";
    if(clientMap.find(user) == clientMap.end()){
        /* Novo usuário */
        clientMap[user] = Client(user, -1);
    }

    if(clientMap.find(user)->second.socketfd == -1){
        /* Associa socket ao usuário*/
        clientMap[user].socketfd = sockfd;
        connectedClientMap[sockfd] = &clientMap[user];
    } else {
        /* Usuário com um socket associado já*/
        connectedClientMap[sockfd] = new Client("", sockfd);
        status = "CONN Usuário já conectado\n";
    }

    Client* client = connectedClientMap[sockfd];
    message.setBuffer(status.data(), status.length());
    message.origin = client;
    message.dest = client;

    /* Envia mensagem para usuário conectado */
    message.sendMessage();
    if(client->user == ""){
        /* Fecha conexão com socket sem usuário */
        Server::closeSocket(sockfd);
    }
}

// Referência: http://stackoverflow.com/questions/212528/get-the-ip-address-of-the-machine
void Server::printLocalAddress() {
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    LOG(INFO) <<  "####### Server IPs ########";

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            LOG(INFO) <<  ifa->ifa_name << ": " << addressBuffer << ":" << Server::port;
        }
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);

    LOG(INFO) <<  "####### Server IPs ######## \n";
}



