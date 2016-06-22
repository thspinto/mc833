/*
* Code from http://www.unpbook.com/
* 
* 
*/

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
	int					i, maxi, maxfd, tcpfd, udpfd, new_s, sockfd, len;
	int 				pid;
	ssize_t				n;
	fd_set				rset, allset;
	char				buf[MAXLINE];
	socklen_t			clilen;
	const int on = 1;
	struct sockaddr_in	cliaddr, servaddr;

	if ((tcpfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket error");
		return 1;
	}
	
	
	if ((udpfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket error");
		return 1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	setsockopt(tcpfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if (bind(tcpfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)  {
		perror("bind error");
		close(tcpfd);
		close(udpfd);
		return 1;
	}
	
	if (bind(udpfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)  {
		perror("bind error");
		close(tcpfd);
		close(udpfd);
		return 1;
	}

	if (listen(tcpfd, LISTENQ) < 0) {
		perror("listen error");
		close(tcpfd);
		close(udpfd);
		return 1;
	}

	maxfd = udpfd;			/* initialize */
	maxi = -1;					/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
	FD_ZERO(&allset);
	FD_SET(udpfd, &allset);
	FD_SET(tcpfd, &allset);

	for ( ; ; ) {
		rset = allset;		/* structure assignment */
		select(maxfd+1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(tcpfd, &rset)) {	/* new client connection */
			clilen = sizeof(cliaddr);
			new_s = accept(tcpfd, (struct sockaddr *) &cliaddr, &clilen);
			
			pid = fork();
			if (pid < 0) {
				perror("simplex-talk: fork");
				exit(1);
			}
			if (pid == 0) {
				close(tcpfd);
				close(udpfd);
				while (len = recv(new_s, buf, sizeof(buf), 0)) {
					printf("[tcp] %s", buf);
					send(new_s, buf, len, 0);
				}
				close(new_s);
				exit(0);
			}
			close(new_s);
		}
		if (FD_ISSET(udpfd, &rset)) {	
					len = recvfrom(udpfd, buf, MAXLINE, 0, (struct sockaddr *)&cliaddr, &clilen);
			if (len > 0) {
				buf[len] = 0;
				printf("[udp] %s", buf);
				sendto(udpfd, buf, strlen(buf), 0, (struct sockaddr *)&cliaddr, clilen);
			}
		}
	}
}
