#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 56789
#define MAX_LINE 256

int main(int argc, char * argv[]){

	FILE *fp;
	struct hostent *hp;
	struct sockaddr_in servaddr, cliaddr;
	char *host, *type;
	char buf[MAX_LINE];
	int s, len, recvlen, addr_len = sizeof(servaddr);
	if (argc==3) {
		host = argv[1];
		type = argv[2];
	}else {
		fprintf(stderr, "usage: ./client host (tcp or udp)\n");
		exit(1);
	}


	/* translate host name into peers IP address */
	hp = gethostbyname(host);
	if (!hp) {
		fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
		exit(1);
	}

	/* build address data structure */
	bzero((char *)&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	bcopy(hp->h_addr, (char *)&servaddr.sin_addr, hp->h_length);
	servaddr.sin_port = htons(SERVER_PORT);

	/* active open */
	if (strcmp(type,"tcp") == 0){
		s = socket(PF_INET, SOCK_STREAM, 0);
	}else if (strcmp(type,"udp") == 0){
		s = socket(PF_INET, SOCK_DGRAM, 0);
	}else{
		printf("simplex-talk: connection error protocol undefined!!\n");
		exit(1);
	}
		
	if (s < 0) {
		perror("simplex-talk: socket");
		exit(1);
	}

	if (connect(s, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("simplex-talk: connect");
		close(s);
		exit(1);
	}

	/* main loop: get and send lines of text */
	while (fgets(buf, sizeof(buf), stdin)) {
		buf[MAX_LINE-1] = '\0';
		len = strlen(buf) + 1;
		send(s, buf, len, 0);
		while(1){
			if ((recvlen = recv(s, buf, sizeof(buf), 0)) < 0){
				perror("simplex-talk: connect");
				close(s);
				exit(1);
			}
			buf[recvlen] = 0;
			printf("received message: %s", buf);
			break;
		}
	}
}
