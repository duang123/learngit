#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include<netinet/in.h>
#include<errno.h>

#define ERR_EXIT(m)\
		{perror(m);\
		exit(1);\
		}

ssize_t writen(int fd, const void *buf, size_t count){
	char* bufp=(char*)buf;
	size_t nleft=count;
	ssize_t written;

	while(nleft>0){
		if((written=write(fd,bufp,nleft))<0){
			if(errno==EINTR)continue;//信号中断
				return -1;
		}
		else if(written==0)continue;
		bufp+=written;
		nleft-=written;
	}
	return count;
}

ssize_t readn(int fd, void *buf, size_t count){
	char* bufp=(char*)buf;
	size_t nleft=count;
	ssize_t nread;
	while(nleft>0){
		if((nread=read(fd,bufp,nleft))<0){
			if(errno==EINTR)continue;	//信号中断
				return -1;
		}
		else if(nread==0)return count-nleft;
		bufp+=nread;
		nleft-=nread;
	}
	return count;
}

ssize_t recv_peek(int sockfd, void *buf, size_t len){
	while(1){
		char* bufp=buf;
		int ret=recv(sockfd,bufp,len,MSG_PEEK);
		if(ret==-1 && errno==EINTR)continue;
		return ret;
	}
}

ssize_t readline(int sockfd,void* buf,size_t maxline){
	char* bufp=buf;
	int nread;
	int nleft=maxline;
	int ret;

	while(1){
		ret=recv_peek(sockfd,bufp,nleft);
		if(ret==-1){
			return ret;
		}
		nread=ret;
		int i;
		for(i=0;i<nread;++i){
			if(bufp[i]=='\n'){
				ret=readn(sockfd,bufp,i+1);
				if(ret==-1){return ret;}
				else if(ret!=i+1){return ret;}
				return ret;
			}
		}
		//偷窥到ret个数据，现在开始读数据
		if(nread>nleft)exit(1);
		ret=readn(sockfd,bufp,nread);
		if(ret==-1){exit(1);}
		else if(ret!=nread){exit(1);}
		bufp+=nread;
		nleft-=nread;
	}
	return -1;
}

void echo_clt(int sock){
	char recevbuf[1024]={0};
	char sendbuf[1024]={0};
	memset(sendbuf,0,sizeof(sendbuf));
	memset(recevbuf,0,sizeof(recevbuf));
	int n;
	while(fgets(sendbuf,sizeof(sendbuf),stdin)!=NULL){
		n=strlen(sendbuf);
		writen(sock,sendbuf,n);

		int ret=readline(sock,recevbuf,n);
		if(ret==-1){
			ERR_EXIT("readn");
		}
		else if(ret==0){
			printf("server close\n");
			break;
		}
		fputs(recevbuf,stdout);
		memset(sendbuf,0,sizeof(sendbuf));
		memset(recevbuf,0,sizeof(recevbuf));
	}

}
int main(){

	int sock;
	if((sock=socket(AF_INET,SOCK_STREAM,0))<0){
		ERR_EXIT("socket");
	}

	struct sockaddr_in severaddr;
	severaddr.sin_family=AF_INET;
	severaddr.sin_port=5656;
	severaddr.sin_addr.s_addr=inet_addr("58.198.84.205");
//    severaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	//inet_aton("127.0.0.1",&severaddr.sin_addr.s_addr);
	if((connect(sock,(struct sockaddr*)&severaddr,sizeof(severaddr)))<0){
		ERR_EXIT("accept");
	}
	
	struct sockaddr_in localaddr;
	socklen_t localaddr_len=sizeof(localaddr);
	if(getsockname(sock,(struct sockaddr*)&localaddr,&localaddr_len)<0){
		ERR_EXIT("getsockname");
	}
	printf("local ip=%s,port=%d\n",inet_ntoa(localaddr.sin_addr),ntohs(localaddr.sin_port));
	echo_cli(sock);	//客户端与服务器通信细节使用echo_clt函数封装
	close(sock);
	return 0;	

}
