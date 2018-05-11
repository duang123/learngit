#include"common.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>
#include<string.h>

void str_echo(int sockfd)
{
	ssize_t n;
	char buf[1024]={0};
	while(1)
	{
		n=read(sockfd,buf,1024);
		if(n==-1)
		{
			if(errno==EINTR)
				continue;
			ERR_EXIT("read");	
	    }
		if(n==0)
		{
			printf("peer close\n");
			break;
		}
		writen(sockfd,buf,n);
		memset(buf,0,sizeof(buf));
	}
}

static void* doit(void*);

int main()
{
	int listenfd,*iptr;
	pthread_t tid;
	socklen_t len;
	struct sockaddr_in cliaddr,servaddr;

	listenfd=socket(AF_INET,SOCK_STREAM,0);
	if(listenfd==-1)
		ERR_EXIT("socket");
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=2525;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	listen(listenfd,SOMAXCONN);
	while(1)
	{
		len=sizeof(servaddr);
		iptr=malloc(sizeof(int));
		*iptr=accept(listenfd,(struct sockaddr*)&cliaddr,&len);
		pthread_create(&tid,NULL,&doit,iptr);
	}
}

static void* doit(void *arg)
{
	int connfd;
	connfd=*((int*)arg);
	free(arg);
	pthread_detach(pthread_self());
	str_echo(connfd);
	close(connfd);
	return (NULL);
}
