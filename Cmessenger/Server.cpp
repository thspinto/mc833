#include "Server.h"
#include "easylogging++.h"

int Server::run(int port) {
    //Cria cliente que representa o servidor
    clientMap["Server"] = User("Server", -1);

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
    }
    for(std::list<Message>::iterator it = messageQueue.begin(); it != messageQueue.end(); it++){
        if(it->dest->socketfd != -1){
            it->sendMessage();
            messageQueue.erase(it);
            sendDeliveryNotification(*it);
        }
    }
}

void Server::sendDeliveryNotification(Message &message) {
    std::string status = message.id;
    status.append(" entregue\n");
    if(message.origin->user != "Server") {
        Message *deliverMessage = NULL;
        if (message.groupHeader.length() > 0) {
            int count = --groupMap[message.groupHeader].messageCount[message.id];
            if (count == 0) {//Grupo
                std::vector<char> statusBuffer(status.begin(), status.end());
                deliverMessage = new Message(&clientMap.find("Server")->second, message.origin, statusBuffer);
            }
        } else {//Individual
            std::__1::vector<char> statusBuffer(status.begin(), status.end());
            deliverMessage = new Message(&clientMap.find("Server")->second, message.origin, statusBuffer);
        }
        if(deliverMessage != NULL) {
            if (deliverMessage->dest->socketfd != -1) {
                deliverMessage->sendMessage();
            } else {
                messageQueue.push_front(*deliverMessage);
            }
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
        if(it->second.user == "Server") {
            /* Ingnora o usuário server */
            continue;
        }
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

std::string Server::getMessageId(std::string destName, Message &message) {
    std::string messageId = message.origin->user;
    messageId.append(destName).append(message.toString());
    return md5(messageId).substr(0, 6);
}

void Server::sendg(Message &message) {
    if(!verifyConnectedClient()){
        return;
    };
    std::string status = ("Groupo não existe\n");
    std::string groupName = message.parseCommandParameter();
    const std::map<std::string, Group>::iterator &it = groupMap.find(groupName);
    if(it != groupMap.end()) {
        Group group = it->second;
        message.id = getMessageId(groupName, message);
        status = message.id.append(" enfileirada\n");
        User* origin = connectedClientMap[sockfd];
        group.messageCount[message.id] = 0;
        std::set<User *>::iterator clientsIt;
        for(clientsIt = group.clients.begin(); clientsIt != group.clients.end(); clientsIt++) {
            if ((*clientsIt)->user != origin->user) { //Não envia para a origem
                Message groupMessage(origin, *clientsIt, message.buf);
                groupMessage.groupHeader = groupName;
                messageQueue.push_back(groupMessage);
                group.messageCount[message.id]++;
            }
        }
    }

    sendServerMessage(status);
}

void Server::joing(Message &message) {
    if(!verifyConnectedClient()){
        return;
    };
    std::string status = ("Groupo não existe\n");
    std::string groupName = message.parseCommandParameter();
    const std::map<std::string, Group>::iterator &it = groupMap.find(groupName);
    if(it != groupMap.end()) {
        status = ("Usuário adicionado ao grupo ");
        status.append(groupName).append("\n");
        it->second.clients.insert(connectedClientMap[sockfd]);
    }

    sendServerMessage(status);
    }

void Server::createg(Message &message) {
    if(!verifyConnectedClient()){
        return;
    };

    std::string status = ("Groupo já existe\n");
    std::string groupName = message.parseCommandParameter();
    if(groupMap.find(groupName) == groupMap.end()) {
        Group group(groupName);
        group.clients.insert(connectedClientMap[sockfd]);
        groupMap[groupName] = group;
        status = ("Groupo ");
        status = status.append(groupName).append(" criado\n");
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
    message.id = getMessageId(destUser, message);

    std::string status = message.id;
    status.append(" enfileirada\n");
    if(message.dest != NULL) {
        messageQueue.push_back(message);
        sendServerMessage(status);
    }
}

bool Server::verifyConnectedClient() {
    if(connectedClientMap.find(sockfd)->second == NULL){
        std::string status = ("Usuário não identificado\n");
        sendServerMessage(status);
        closeSocket(sockfd);
        return false;
    }
    return true;
}

User* Server::verifyDestClient(std::string destUser) {
    if(clientMap.find(destUser) == clientMap.end()){
        std::string status = std::string("Usuário de destino não existente\n");
        sendServerMessage(status);
        return NULL;
    }
    return &clientMap.find(destUser)->second;
}

void Server::sendServerMessage(std::string &status) const {
    std::vector<char> statusBuffer(status.begin(), status.end());
    User *client = connectedClientMap.find(sockfd)->second;
    Message message(&clientMap.find("Server")->second, client, statusBuffer);
    message.sendMessage();
}

void Server::conn(Message &message) {
    std::string user(message.parseCommandParameter());
    std::string status = "Usuário conectado\n";
    if(clientMap.find(user) == clientMap.end()){
        /* Novo usuário */
        clientMap[user] = User(user, -1);
    }
    if(connectedClientMap.find(sockfd) != connectedClientMap.end()) {
        //Já existia um cliente nessa sessão. Desconecta ele.
        connectedClientMap.find(sockfd)->second->socketfd = -1;
    }

    if(clientMap.find(user)->second.socketfd == -1){
        /* Associa socket ao usuário*/
        clientMap[user].socketfd = sockfd;
        connectedClientMap[sockfd] = &clientMap[user];
    } else {
        /* Usuário com um socket associado já*/
        status = "Usuário já conectado\n";
    }

    sendServerMessage(status);

    User* client = connectedClientMap[sockfd];
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



