#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <stdio.h>
#include<stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define ERR_EXIT(m){ \
		perror(m); \
		exit(-1); \
       }\

void echo_svr(int sock)
{
	char recvbuf[1024]={0};	
	int n;
	struct sockaddr_in cliaddr;
	socklen_t clilen=sizeof(cliaddr);
	while(1)
	{	
		n=recvfrom(sock,recvbuf,sizeof(recvbuf),0,(struct sockaddr*)&cliaddr,&clilen);
		if(n==-1)
			ERR_EXIT("recvfrom");
		fputs(recvbuf,stdout);
		sendto(sock,recvbuf,strlen(recvbuf),0,(struct sockaddr*)&cliaddr,clilen);
	}
	close(sock);
}

int main()
{
	int sockfd;
	struct sockaddr_in servaddr;

	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd==-1){
		ERR_EXIT("socket");
	}
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=2525;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	if(bind(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr))==-1){
		ERR_EXIT("bind");
	}
	echo_svr(sockfd);

}
