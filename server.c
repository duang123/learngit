#include<string.h>
#include<stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include<netinet/in.h>
#include <arpa/inet.h>

#define ERR_EXIT(m)\
		{perror(m);\
		exit(1);\
		}


int main(){

	int listenfd;
	if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0){
		ERR_EXIT("socket");
	}

	struct sockaddr_in severaddr;
	severaddr.sin_family=AF_INET;
	severaddr.sin_port=5656;
//	severaddr.sin_addr.s_addr_t=inet_addr("58.198.84.205");
    severaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	//inet_aton("127.0.0.1",&severaddr.sin_addr.s_addr);
	int on=1;
	if((setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0){
		ERR_EXIT("setsockopt");
	}//设置地址重复利用
	if(bind(listenfd,(struct sockaddr*)&severaddr,sizeof(severaddr))<0){
		ERR_EXIT("bind");
	}
	if(listen(listenfd,SOMAXCONN)<0){
		ERR_EXIT("listen");
	}
	struct sockaddr_in peeraddr;
	socklen_t peerlen=sizeof(peeraddr);
	int conn;
	if((conn=accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen))<0){
		ERR_EXIT("accept");
	}
	printf("ip=%s,port=%d\n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));
	char recevbuf[1024];
	memset(recevbuf,0,sizeof(recevbuf));
	while(1){
		int ret=read(conn,recevbuf,sizeof(recevbuf));
		fputs(recevbuf,stdout);
		write(conn,recevbuf,ret);
	    memset(recevbuf,0,sizeof(recevbuf));
	}
	close(conn);
	close(listenfd);
	return 0;	

}
