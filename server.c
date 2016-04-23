#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>


#define SERVER_PORT 31472
#define MAX_PENDING 5
#define MAX_LINE 256

int main()
{
	struct sockaddr_in sin, addr;
	char buf[MAX_LINE];
	int len;
	int s, new_s, pid;

	/* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(SERVER_PORT);

	/* setup passive open */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("simplex-talk: socket");
		exit(1);
	}
	if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
		perror("simplex-talk: bind");
		exit(1);
	}
	listen(s, MAX_PENDING);

	/* wait for connection, then receive and print text */
	while(1) {
		if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
			perror("simplex-talk: accept");
			exit(1);
		}

		len = sizeof(addr);
		if(getpeername(new_s, &addr, &len) < 0){
			perror("simplex-talk: getsockname");
		}
		printf("Socket %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

		pid = fork();
		if (pid < 0) {
			 perror("simplex-talk: fork");
			 exit(1);
		}
		if (pid == 0) {
			close(s);
			while (len = recv(new_s, buf, sizeof(buf), 0)) {
				fputs(buf, stdout);
				send(new_s, buf, len, 0);
			}
			close(new_s);
			exit(0);
		}
		close(new_s);
	}
}
