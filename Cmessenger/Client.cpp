#include <string.h>
#include <iostream>
#include <vector>
#include "Client.h"


int Client::run(){
    if(createConnection()){
        getAndSendMessages();
    }

    return 0;
}

bool Client::createConnection(){
    struct sockaddr_in sin;
    struct hostent *hp;
    hp = gethostbyname(Client::host);

    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    sin.sin_port = htons(Client::serverPort);

    if ((Client::socketfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("simplex-talk: socket");
        return false;
    }

    if (connect(Client::socketfd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("simplex-talk: connect");
        close(Client::socketfd);
        return false;
    }

    return true;
}

// Evnia mensagem para o servidor
void Client::sendMessage(std::string message){
    std::string size = std::to_string(message.size());
    std::string msg = size.append(" ").append(message);
    send(socketfd, &msg[0], msg.length(), 0);
    return;
}

// Recebe mensagem do servidor e exiba na tela
void Client::recvMessage(){
    std::vector<char> buf(MAX_LINE);
    recv(socketfd, buf.data(), buf.size(), 0);
    std::cout << buf.data();
    return;
}

int Client::getAndSendMessages(){
    std::string init = "";
    char aux[MAX_LINE];

    //Inicia conexao com o servidor
    init.append("CONN ");
    init.append(Client::user);
    sendMessage(init);
    recvMessage();
    std::cout << "\n" << "$[" << Client::user << "] ";
    while (std::cin.getline(aux,MAX_LINE)){
        aux[MAX_LINE-1] = '\0';
        std::cout << "\n";
        sendMessage(aux);
        recvMessage();
        std::cout << "\n" << "$[" << Client::user << "] ";
    }
}