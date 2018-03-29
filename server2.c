#include<string.h>
#include<stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define ERR_EXIT(m)\
		{perror(m);\
		exit(1);\
		}

void do_service(int conn){
	      char recevbuf[1024];
	      while(1){
	       memset(recevbuf,0,sizeof(recevbuf));
		   int ret=read(conn,recevbuf,sizeof(recevbuf));
		   if(ret==0){
		   		printf("client close\n");
				break;
		   }
		   else if(ret==-1){
		   	ERR_EXIT("read");
		   }
		   fputs(recevbuf,stdout);
		   write(conn,recevbuf,ret);
	      }
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
	pid_t pid;
    while(1){
	    if((conn=accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen))<0)          {
		    ERR_EXIT("accept");
	    }
	    printf("ip=%s,port=%d\n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));
		
		pid=fork();
		if(pid<0){
			ERR_EXIT("fork");
		}
		else if(pid==0){
		  close(listenfd);
		  do_service(conn);
		  close(conn);
		  exit(0);
		}
		else {
			close(conn);
		}
	}
	return 0;	

}
