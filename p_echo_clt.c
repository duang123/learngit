 #include <sys/types.h>          /* See NOTES */
 #include <sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include <errno.h>
 #include <netinet/in.h>
#include <arpa/inet.h>
#include"common.h"
#include<pthread.h>
#include<string.h>

void* copyto(void*);
static int sockfd;
static FILE* fp;

void str_cli(FILE* fp_arg,int sock_arg)
{
	char recvline[1024]={0};
	pthread_t tid;
	sockfd=sock_arg;
	fp=fp_arg;
	pthread_create(&tid,NULL,copyto,NULL);
	while(readline(sockfd,recvline,1024)>0)
	{
			fputs(recvline,stdout);
			memset(recvline,0,sizeof(recvline));
	}
}

void* copyto(void* arg)
{
	char sendline[1024]={0};
	while(fgets(sendline,1024,fp)!=NULL)
	{
		writen(sockfd,sendline,strlen(sendline));
		memset(sendline,0,sizeof(sendline));
	}
	shutdown(sockfd,SHUT_WR);
	return (NULL);
}

int main()
{
	struct sockaddr_in servaddr;
	int sockfd;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd==-1)
		ERR_EXIT("socket");
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=2525;
	servaddr.sin_addr.s_addr=inet_addr("58.198.84.205");
	connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));		
	str_cli(stdin,sockfd);
	close(sockfd);
}
