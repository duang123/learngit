#include"sysutil.h"
#include <sys/socket.h>
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include <errno.h>
#include <arpa/inet.h>
#include<netinet/in.h>

#define ERR_EXIT(m)\
		{perror(m);\
		exit(1);\
		}

int main(){
	int sockfd;
	struct sockaddr_in servaddr;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd==-1){
		ERR_EXIT("socket");
	}
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_addr.s_addr=inet_addr("58.198.84.205");
	servaddr.sin_port=htons(2525);
	servaddr.sin_family=AF_INET;

	int ret;
	ret=connect_timeout(sockfd,&servaddr,10);
	if(ret==-1&&errno==ETIMEDOUT){
		ERR_EXIT("TIMEOUT");
	}else if(ret==-1){
		ERR_EXIT("connect_rimeout");
	}
	struct sockaddr_in local;
	socklen_t len=sizeof(local);
	memset(&local,0,len);
	ret=getsockname(sockfd,(struct sockaddr*)&local,&len);
	if(ret==-1){
		ERR_EXIT("getsockname");
	}

	printf("local ip=%s,port=%d\n",inet_ntoa(local.sin_addr),local.sin_port);
}
