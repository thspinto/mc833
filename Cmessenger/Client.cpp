#include <string.h>
#include "Client.h"
#include "easylogging++.h"

int Client::run(char* lh, int port, char* user){

    Client::serverPort = port;
    strcpy(Client::host, lh);
    strcpy(Client::user, user);

    printf("bla....");
    if(createConnection()){
        getAndsendMessages();
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

    if ((Client::socketfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
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

int Client::getAndsendMessages(){
    size_t len;
    char buf[MAX_LINE];
    char aux[MAX_LINE];
    std::cout << "$[" << Client::user << "]";
    while (std::cin >> aux) {
        std::cout << "\n";
        //fgets(aux, sizeof(buf), stdin)
        len = strlen(aux);
        sprintf(buf, "%d", len);
        strcat(buf, " ");
        strcat(buf, aux);
        buf[MAX_LINE-1] = '\0';
        len = strlen(buf) + 1;
        send(Client::socketfd, buf, len, 0);
        recv(Client::socketfd, buf, sizeof(buf), 0);
        //fputs(buf, stdout);
        std::cout << buf;
        std::cout << "$[" << Client::user << "]";
    }
}

int main(int argc, char **argv) {
    int port = 4444;
    if(argc > 1) {
        std::istringstream ht(argv[1]);
        if(strlen(ht) < 8){
            //7.7.7.7
            LOG(WARNING) << "Invalid host " << argv[1] << '\n';
            std::cout << "usage hostname, eg: 143.134.65.23";
        }
        std::istringstream iss(argv[2]);
        if (!(iss >> port)) {
            LOG(WARNING) << "Invalid port " << argv[1] << '\n';
            std::cout << "usage: server <port>";
        }

        std::istringstream Client(argv[3]);
        if (!Client){
            LOG(WARNING) << "Invalid port " << argv[1] << '\n';
            std::cout << "usage: your name";
        }
    }

    Client client;
    client.run(argv[1], (int) argv[2], argv[3]);

    return 0;
}