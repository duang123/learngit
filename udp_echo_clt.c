#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include<stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define ERR_EXIT(m){ \
	perror(m);  \
	exit(-1);  \
}\

void echo_cli(int sock,struct sockaddr* servaddr,socklen_t servlen)
{
	int n;
	char recvbuf[1024]={0};
	char sendbuf[1024]={0};
	while(fgets(sendbuf,sizeof(sendbuf),stdin)!=NULL)
	{
		sendto(sock,sendbuf,strlen(sendbuf),0,servaddr,servlen);
		n=recvfrom(sock,recvbuf,sizeof(recvbuf),0,NULL,NULL);
		if(n==-1)
		{
			if(errno==EINTR)
				continue;
			ERR_EXIT("recvbuf");
		}
		fputs(recvbuf,stdout);
		memset(recvbuf,0,sizeof(recvbuf));
		memset(sendbuf,0,sizeof(sendbuf));
	}
	close(sock);
}

int main()
{
	int sockfd;
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=2525;
	servaddr.sin_addr.s_addr=inet_addr("58.198.84.205");

	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd==-1)
		ERR_EXIT("socket");
	echo_cli(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	exit(0);
}
