#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include<errno.h>
#include<string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/select.h>

#define ERR_EXIT(m)\
		{perror(m);\
		exit(1);\
		}
//这些利用库函数封装的函数，实现某种特定的功能，且对某类错误不加处理，仅仅返回不正确的值。因此当调用这些封装函数时，应加入返回值检查处理。

ssize_t readn(int fd, void *buf, size_t count){
	char* bufp=(char*)buf;
	ssize_t nread;
	size_t nleft=count;

	while(nleft>0){
		if((nread=read(fd,bufp,nleft))<0){
			if(errno==EINTR)continue;//被信号中断
			return -1;
		}
		else if(nread==0)return count-nleft;//读到EOF
		bufp+=nread;
		nleft-=nread;
	}
	return count;
}

ssize_t writen(int fd, const void *buf, size_t count){
	
	char* bufp=(char*)buf;
	ssize_t nwritten;
	size_t nleft=count;

	while(nleft>0){
		if((nwritten=write(fd,bufp,nleft))<0){
			if(errno==EINTR)continue;//被信号中断
			return -1;
		}
		else if(nwritten==0)continue;//继续写
		bufp+=nwritten;
		nleft-=nwritten;
	}
	return count;
}

ssize_t recv_peek(int sockfd, void *buf, size_t len){
		
	while(1){
		int ret=recv(sockfd,buf,len,MSG_PEEK);
		if(ret==-1 && errno==EINTR)//信号中断
			continue;
		return ret;
	}
}

ssize_t readline(int sockfd, void *buf, size_t maxline){
	char* bufp=buf;
	int nread;
	int nleft=maxline;
	int ret;
	while(1){
		ret=recv_peek(sockfd,bufp,nleft);
		if(ret==-1){
			return ret;
		}
		else if(ret==0)return ret;
		int i;	//刚才偷窥到ret个字符，现在开始读
		nread=ret;
		for(i=0;i<nread;++i){
			if(bufp[i]=='\n'){
				ret=readn(sockfd,bufp,i+1);
				if(ret!=i+1){
					return ret;
				}
				return ret;
			}
		}
		if(nread>nleft){
			exit(1);
		}
		ret=readn(sockfd,bufp,nread);
		if(ret!=nread){
			exit(1);	
		}
		bufp+=ret;
		nleft-=ret;
	}
	return -1;
}

void echo_sv(int conn){
	   char recevbuf[1024];
		int n;
		while(1){
	       memset(recevbuf,0,sizeof(recevbuf));
		   int ret=readline(conn,recevbuf,1024);
		   if(ret==-1){
		   	ERR_EXIT("readline");
		   }
		   else if(ret==0){
		   		printf("client close\n");
				break;
		   }
		   fputs(recevbuf,stdout);
		   writen(conn,recevbuf,strlen(recevbuf));
	      }
}

void handler_sigchld(int p){
	while(waitpid(-1,NULL,WNOHANG)>0){
		;				//查看正在运行的进程列表 ps -ef | grep echo_sv
	}
}

void handler_sigpipe(int sig){
	printf("receive a sig=%d\n",sig);
	exit(1);
}

int main(){
	signal(SIGPIPE,handler_sigpipe);
//	signal(SIGCHLD,SIG_ING);
	signal(SIGCHLD,handler_sigchld);	
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
	socklen_t peerlen;
	int conn;
	char recevbuf[1024];

/*	pid_t pid;
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
		  echo_sv(conn);
		  close(conn);
		  exit(0);
		}
		else {
			close(conn);
		}
	}
*/
	fd_set readset;
	fd_set allset;
	FD_ZERO(&readset);
	FD_ZERO(&allset);
	FD_SET(listenfd,&allset);
	int maxfd=listenfd;
	int conn_arr[FD_SETSIZE];
	int i,nfds;
	int maxi=-1;
	for(i=0;i<FD_SETSIZE;++i)conn_arr[i]=-1;
	while(1){
		readset=allset;
		nfds=select(maxfd+1,&readset,NULL,NULL,NULL);
		if(nfds==-1){
			if(errno==EINTR)continue;
				ERR_EXIT("select");
		}else if(nfds==0){continue;}//超时继续

		if(FD_ISSET(listenfd,&readset)){
			peerlen=sizeof(peeraddr);
			conn=accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen);	
			if(conn==-1){
				ERR_EXIT("accept");
			}
			int j=0;
			for(j=0;j<FD_SETSIZE;++j){
				if(conn_arr[j]==-1){
					conn_arr[j]=conn;
					break;
				}
			}
			if(j==FD_SETSIZE){
				ERR_EXIT("too many client");
			}
			printf("peer add=%s,peer port=%d\n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));
			FD_SET(conn,&allset);
			maxfd=(maxfd>conn)?maxfd:conn;
			maxi=maxi>j? maxi:j;
			if(--nfds<=0)continue;
		}
		int j;	
		for(j=0;j<=maxi;++j){
			conn=conn_arr[j];
			if(conn==-1)continue;
			if(FD_ISSET(conn_arr[j],&readset)){
	       		memset(recevbuf,0,sizeof(recevbuf));
		   		int ret=readline(conn,recevbuf,1024);
		   		if(ret==-1){
		   			ERR_EXIT("readline");
		   		}
		   		else if(ret==0){ 
		   			printf("client close\n");
					FD_CLR(conn,&allset);
					conn_arr[j]=-1;
					close(conn);
				}
		   		fputs(recevbuf,stdout);
				sleep(5);
		   		writen(conn,recevbuf,strlen(recevbuf));
				if(--nfds<=0)break;
				
			}	
		}
	} 
	return 0;	

}
