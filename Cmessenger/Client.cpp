#include <string.h>
#include "Client.h"



int Client::run(const char* lh, int port){
    int s;
    printf("bla....");
    s = createConnection(lh, port);
    if ( s >= 0 )
        sendMessage(s);

    return 0;
}

int Client::createConnection(const char* ip, int port){
    struct sockaddr_in sockaddrIn;
    struct sockaddr_in sin;
    struct hostent *hp;
    int s;

    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    sin.sin_port = htons(Client::serverPort);


    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("simplex-talk: socket");
        return s;
    }

    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("simplex-talk: connect");
        close(s);
        return -1;
    }

    return s;
}

int Client::sendMessage(int s){
    size_t len;
    char buf[MAX_LINE];
    char aux[MAX_LINE];

    while (fgets(aux, sizeof(buf), stdin)) {
        len = strlen(aux);
        sprintf(buf, "%d", len);
        strcat(buf, " ");
        strcat(buf, aux);
        buf[MAX_LINE-1] = '\0';
        len = strlen(buf) + 1;
        send(s, buf, len, 0);
        recv(s, buf, sizeof(buf), 0);
        fputs(buf, stdout);
    }
}