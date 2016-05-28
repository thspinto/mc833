#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 56789
#define MAX_LINE 256

int main(int argc, char * argv[])
{
	FILE *fp;
	struct hostent *hp;
	struct sockaddr_in servaddr, cliaddr;
	char *host;
	char buf[MAX_LINE];
	int s, len, recvlen, addr_len = sizeof(servaddr);
	if (argc==2) {
		host = argv[1];
	}
	else {
		fprintf(stderr, "usage: ./client host\n");
	exit(1);
}

	/* translate host name into peerâ€™s IP address */
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
	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("simplex-talk: socket");
		exit(1);
	}

	/* main loop: get and send lines of text */
	while (fgets(buf, sizeof(buf), stdin)) {
		buf[MAX_LINE-1] = '\0';
		len = strlen(buf) + 1;
		sendto(s, buf, len, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		while(1){
			recvlen = recvfrom(s, buf, MAX_LINE, 0, (struct sockaddr *)&cliaddr, &addr_len);
			if (recvlen > 0 && (servaddr.sin_addr.s_addr == cliaddr.sin_addr.s_addr &&
																		servaddr.sin_port == cliaddr.sin_port)) {
							buf[recvlen] = 0;
							printf("received message: %s", buf);
							break;
			}
		}
	}
}
