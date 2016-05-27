#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <string.h>

#define LISTENQ 5
#define MAXLINE 64
#define SERV_PORT 56789

int
main(int argc, char **argv)
{
	int	listenfd, recvlen;
	char buf[MAXLINE];
	struct sockaddr_in cliaddr, servaddr;
	socklen_t	clilen = sizeof(cliaddr);

	if ((listenfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket error");
		return 1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)  {
		perror("bind error");
		close(listenfd);
		return 1;
	}

	for (;;) {
		recvlen = recvfrom(listenfd, buf, MAXLINE, 0, (struct sockaddr *)&cliaddr, &clilen);
		if (recvlen > 0) {
						buf[recvlen] = 0;
						printf("received message: %s", buf);
						sendto(listenfd, buf, strlen(buf), 0, (struct sockaddr *)&cliaddr, clilen);
		}
	}
}
