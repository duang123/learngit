#include<string.h>
#include<stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define ERR_EXIT(m)\
		{perror(m);\
		exit(1);\
		}

void handler(int sig){
	printf("receive a signal=%d\n",sig);
	exit(0);
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
	
	pid_t pid;
	pid=fork();
	if(pid==-1){
		ERR_EXIT("fork");
	}
	else if(pid==0){
		signal(SIGUSR1,handler);
		char sendbuf[1024]={0};
		while(fgets(sendbuf,sizeof(sendbuf),stdin)!=NULL){
			int ret=write(conn,sendbuf,sizeof(sendbuf));
			memset(sendbuf,0,sizeof(sendbuf));
		}
		printf("child close\n");
		exit(0);
	}
	else {//父进程负责接受收据 
	    char recevbuf[1024];
		while(1){
	    	memset(recevbuf,0,sizeof(recevbuf));
			int ret=read(conn,recevbuf,sizeof(recevbuf));
			if(ret==-1){
				ERR_EXIT("read");
			}
			else if(ret==0){
				printf("client close\n");
				break;	//客户端关闭，服务器也停止服务
			}
			fputs(recevbuf,stdout);
		}
		close(conn);
		printf("parent close\n");
		kill(pid,SIGUSR1);
		ERR_EXIT(0);
	}
	return 0;	

}
