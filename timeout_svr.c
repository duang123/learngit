#include"sysutil.h"
#include <sys/socket.h>
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include <errno.h>
#include <arpa/inet.h>
#include<netinet/in.h>
#include <sys/types.h>  

#define ERR_EXIT(m)\
         {perror(m);\
         exit(1);\
        }

int main(){
	int listenfd,connfd;
	struct sockaddr_in  servaddr,cliaddr;

	listenfd=socket(AF_INET,SOCK_STREAM,0);
	if(listenfd==-1){
        ERR_EXIT("socket");
	}   

	 memset(&servaddr,0,sizeof(servaddr));
     servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	 servaddr.sin_port=htons(2525);
	 servaddr.sin_family=AF_INET;
	 int on=1;
	 if(setsockopt(listenfd,SOL_SOCKET, SO_REUSEADDR,&on,sizeof(on))<0){
	 	ERR_EXIT("setsockopt");
	 }//设置地址重复利用

	 int ret;
	 ret=bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	 if(ret==-1){
	 	ERR_EXIT("bind");
	 }
	 ret=listen(listenfd, SOMAXCONN);
	 if(ret==-1){
	 	ERR_EXIT("listen");
	 }

	 ret=accept(listenfd,&cliaddr,10);
	 if(ret==-1){
	 	ERR_EXIT("accept_timeout");
	 }
	printf("peer addr=%s,peer port=%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);

}
