#include <sys/socket.h>
#include<sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/wait.h>

#define ERR_EXIT(m)\
{\
	perror(m);\
	exit(1);\
}\

//ssize_t write (int fd,const void * buf,size_t count);
ssize_t writePipe(int fd,const void * buf,size_t count)
{
	char* buf1=(char*)buf;
	ssize_t n=write(fd,buf1,count);
	if(n==-1)
		ERR_EXIT("writePipe");
	return n;
}

void readPipe(int fd)
{
	char buf[1024]={0};
	ssize_t n;
	while(1)
	{
		n=read(fd,buf,1024);
		fputs(buf,stdout);
		memset(buf,0,sizeof(buf));
	}
}

int main()
{
	int pipe1[2];
	pid_t childpid;
	int listenfd;
	socklen_t len;
	struct sockaddr_in cliaddr,servaddr;
	listenfd=socket(AF_INET,SOCK_DGRAM,0);
	if(listenfd==-1)
		ERR_EXIT("socket");
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=2525;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));

	int n;
	char mesg[1024]={0};
	len=sizeof(servaddr);
	n=recvfrom(listenfd,mesg,1024,0,(struct sockaddr*)&cliaddr,&len);
	if(n==-1)
		ERR_EXIT("recvfrom");
	printf("get conn\n");
	printf("%s\n",mesg);
	memset(mesg,0,sizeof(mesg));
	n=pipe(pipe1);
	if(n==-1)
		ERR_EXIT("pipe");
	if((childpid=fork())==0)
	{
		close(pipe1[1]);
		readPipe(pipe1[0]);
		exit(0);
	}
	close(pipe1[0]);
	while(1)
	{
		n=recvfrom(listenfd,mesg,1024,0,(struct sockaddr*)&cliaddr,&len);
		if(n==-1)
			ERR_EXIT("recvfrom");
		writePipe(pipe1[1],mesg,n);
		memset(mesg,0,sizeof(mesg));
	}
	waitpid(childpid,NULL,0);
	exit(0);
}
