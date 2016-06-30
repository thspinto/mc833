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

    maxfd = listenfd;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    while(1){
        rset = allset;
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);

        // Verifica se novo cliente conectou
        if (FD_ISSET(listenfd, &rset)) {
            clilen = sizeof cliaddr;
            if((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) == -1){
                perror("accept");
                continue;
            }

            connectedClientMap[connfd] = new User("", connfd);
            FD_SET(connfd, &allset);
            if (connfd > maxfd) {
                maxfd = connfd;
            }

            LOG(INFO) << "User Connected: " << inet_ntoa(cliaddr.sin_addr) << ":" << cliaddr.sin_port;

            if (--nready <= 0) {
                continue;
            }
        }

        //Recebe mensagens dos clientes conectados
        for(std::map<int, User*>::iterator it = connectedClientMap.begin();
            it != connectedClientMap.end(); it++) {
            if(connectedClientMap.size() < 1){
                continue;
            }

            sockfd = it->first;
            if (FD_ISSET(sockfd, &rset)) {
                std::vector<char> buf(MAXLINE);
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
                        message.id = getMessageId(message);
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

    LOG(INFO) << "User disconnected: " << inet_ntoa(cliaddr.sin_addr) << ":" << cliaddr.sin_port;

    close(sockfd);
    FD_CLR(sockfd, &allset);

    User* client = connectedClientMap.find(sockfd)->second;
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
            Server::send(message);
            break;
        case Message::CREATEG :
            Server::createg(message);
            break;
        case Message::JOING :
            Server::joing(message);
            break;
        case Message::SENDG :
            Server::sendg(message);
            break;
        case Message::WHO :
            Server::who();
            break;
        case Message::SENDF:
            Server::sendf(message);
            break;
        case Message::GETF:
            Server::getf(message);
            break;
        default:
            closeSocket(sockfd);
    }

    if(messageQueue.size() > 0) {
        std::list<Message>::iterator it = messageQueue.begin();
        while (it != messageQueue.end()) {
            if (it->dest->socketfd != -1) {
                it->sendMessage();
                sendDeliveryNotification(*it);
                it = messageQueue.erase(it);
                continue;
            }
            it++;
        }
    }
}

void Server::sendf(Message& message) {
    //Salva arquivo localentmente
    //Guarda nome do arquivo no mapa idMensagem -> nome arquivo
    //Manda aviso para cliente de destino
}

void Server::getf(Message& message) {
    //Cria um fork
    //Pega nome do arquivo no mapa
    //Envia o arquivo
    //Apaga o arquivo
    //Envia mensage de arquivo entregue para usuário origem
}

void Server::sendDeliveryNotification(Message &message) {
    std::string status(message.id);
    status.append(" entregue");
    if(message.origin != NULL) {
        if (message.groupHeader.length() > 0) {
            Group group = groupMap[message.groupHeader];
            int count = --group.messageCount.at(message.groupMessageId);
            if (count == 0) {//Grupo
                status.assign(message.originalMessageHash).append(" entregue");
                std::vector<char> statusBuffer(status.begin(), status.end());
                messageQueue.push_back(Message(NULL, message.origin, statusBuffer));
            }
        } else {//Individual
            std::vector<char> statusBuffer(status.begin(), status.end());
            messageQueue.push_back(Message(NULL, message.origin, statusBuffer));
        }
    }
}

void Server::who() {
    if(!verifyConnectedClient()){
        return;
    };
    std::map<std::string, User>::iterator it;
    std::string clientList = "| usuário | status |\n";
    for(it = clientMap.begin(); it != clientMap.end(); it++){
        clientList.append("| ");
        clientList.append(it->first);
        clientList.append(" | ");
        if(it->second.socketfd == -1){
            clientList.append("offline");
        } else {
            clientList.append("online");
        }
        clientList.append(" |\n");
    }
    sendServerMessage(clientList);
}

std::string Server::getMessageId(Message &message) {
    std::string m(message.buf.begin(), message.buf.end());
    return md5(m.substr(0,message.size)).substr(0, 6);
}

void Server::sendg(Message &message) {
    if(!verifyConnectedClient()){
        return;
    };
    std::string status = ("Grupo não existe");
    std::string groupName = message.parseCommandParameter();
    const std::map<std::string, Group>::iterator &it = groupMap.find(groupName);
    if(it != groupMap.end()) {
        if(it->second.clients.find(connectedClientMap[sockfd]) == it->second.clients.end()) {
            status = "Usuário não pertence ao grupo";
        } else if(it->second.clients.size() > 1) {
            Group& group = it->second;
            status.assign(message.id).append(" enfileirada");
            User *origin = connectedClientMap[sockfd];
            group.messageCount.push_back(0);
            std::set<User *>::iterator clientsIt;
            for (clientsIt = group.clients.begin(); clientsIt != group.clients.end(); clientsIt++) {
                if ((*clientsIt)->user != origin->user) { //Não envia para a origem
                    Message groupMessage(origin, *clientsIt, message.buf);
                    groupMessage.size--;
                    groupMessage.originalMessageHash = message.id;
                    groupMessage.groupHeader = groupName;
                    groupMessage.groupMessageId = group.messageCount.size() - 1;
                    messageQueue.push_back(groupMessage);
                    group.messageCount.at(groupMessage.groupMessageId)++;
                }
            }
        } else {
            status = "Grupo vazio";
        }
    }

    sendServerMessage(status);
}

void Server::joing(Message &message) {
    if(!verifyConnectedClient()){
        return;
    };
    std::string status = "Grupo não existe";
    std::string groupName = message.parseCommandParameter();
    const std::map<std::string, Group>::iterator &it = groupMap.find(groupName);
    if(it != groupMap.end()) {
        status = ("Usuário adicionado ao grupo ");
        status.append(groupName);
        it->second.clients.insert(connectedClientMap[sockfd]);
    }

    sendServerMessage(status);
    }

void Server::createg(Message &message) {
    if(!verifyConnectedClient()){
        return;
    };

    std::string status = ("Grupo já existe");
    std::string groupName = message.parseCommandParameter();
    if(groupMap.find(groupName) == groupMap.end()) {
        Group group(groupName);
        group.clients.insert(connectedClientMap[sockfd]);
        groupMap[groupName] = group;
        status = ("Grupo ");
        status = status.append(groupName).append(" criado");
    }
    sendServerMessage(status);
}

void Server::send(Message &message) {
    if(!verifyConnectedClient()){
        return;
    };

    std::string destUser = message.parseCommandParameter();
    message.origin = connectedClientMap[sockfd];
    message.dest = verifyDestClient(destUser);

    std::string status(message.id);
    status.append(" enfileirada");
    if(message.dest != NULL) {
        messageQueue.push_back(message);
        sendServerMessage(status);
    }
}

bool Server::verifyConnectedClient() {
    if(connectedClientMap.find(sockfd)->second == NULL){
        std::string status = ("Usuário não identificado");
        sendServerMessage(status);
        shutdown(sockfd, SHUT_WR);
        return false;
    }
    return true;
}

User* Server::verifyDestClient(std::string destUser) {
    if(clientMap.find(destUser) == clientMap.end()){
        std::string status = std::string("Usuário de destino não existente");
        sendServerMessage(status);
        return NULL;
    }
    return &clientMap.find(destUser)->second;
}

void Server::sendServerMessage(std::string &status) const {
    std::vector<char> statusBuffer(status.begin(), status.end());
    User *client = connectedClientMap.find(sockfd)->second;
    Message message(NULL, client, statusBuffer);
    message.sendMessage();
}

void Server::conn(Message &message) {
    std::string user(message.parseCommandParameter());
    std::string status = user;
    status.append(" conectado");
    if(clientMap.find(user) == clientMap.end()){
        /* Novo usuário */
        clientMap[user] = User(user, -1);
    }

    if(clientMap.find(user)->second.socketfd == -1){
        /* Associa socket ao usuário*/
        clientMap[user].socketfd = sockfd;
        connectedClientMap[sockfd] = &clientMap[user];
    } else {
        /* Usuário com um socket associado já*/
        status = "Usuário já conectado";
    }
    sendServerMessage(status);

    User* client = connectedClientMap[sockfd];
    if(client->user == ""){
        /* Fecha conexão com socket sem usuário */
        shutdown(sockfd, SHUT_WR);
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



