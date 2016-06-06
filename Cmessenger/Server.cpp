#include "Server.h"
#include "easylogging++.h"

int Server::run(int port) {
    Server::port = port;
    int listenfd = startListen();
    selectLoop(listenfd);

    return 0;
}

int Server::startListen() {
    int	listenfd;
    struct sockaddr_in servaddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        return 1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(Server::port);

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)  {
        perror("bind error");
        close(listenfd);
        return 1;
    }

    if (listen(listenfd, LISTENQ) < 0) {
        perror("listen error");
        close(listenfd);
        return 1;
    }
    Server::printLocalAddress();
    return listenfd;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void Server::selectLoop(int listenfd) {
    int maxfd, connfd, nready, n, sockfd;
    fd_set rset, allset;
    socklen_t clilen;
    struct sockaddr_in	cliaddr;
    char buf[MAXLINE];

    maxfd = listenfd;
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    while(1){
        rset = allset;
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);

        // Verifica se novo cliente conectou
        if (FD_ISSET(listenfd, &rset)) {
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
            connectedClientMap[connfd] = Client();

            FD_SET(connfd, &allset);
            if (connfd > maxfd)
                maxfd = connfd;

            if (--nready <= 0)
                continue;
        }

        //Recebe mensagens dos clientes conectados
        for(std::map<int, Client>::iterator it = connectedClientMap.begin();
            it != connectedClientMap.end(); it++) {
            if(connectedClientMap.size() < 1){
                continue;
            }

            sockfd = it->first;
            if (FD_ISSET(sockfd, &rset)) {
                if ((n = read(sockfd, buf, MAXLINE)) == 0) {
                    //Cliente desconectou
                    close(sockfd);
                    FD_CLR(sockfd, &allset);
                    connectedClientMap.erase(it);
                } else {
                    send(sockfd, buf, n, 0);
                    //TODO: Ler mensagem e colocar na fila
                }
                if (--nready <= 0)
                    break; //sem clientes conectados
            }
        }
    }
}
#pragma clang diagnostic pop

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



