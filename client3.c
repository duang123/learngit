#include<string.h>
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

struct packet{
	int len;
	char buf[1024];
};

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

	struct packet recevbuf;
	struct packet sendbuf;
	memset(&sendbuf,0,sizeof(sendbuf));
	memset(&recevbuf,0,sizeof(recevbuf));
	int n;
	while(fgets(sendbuf.buf,sizeof(sendbuf.buf),stdin)!=NULL){
		n=strlen(sendbuf.buf);
		sendbuf.len=htonl(n);
		writen(sock,&sendbuf,n+4);

		int ret=readn(sock,&recevbuf.len,4);
		if(ret==-1){
			ERR_EXIT("readn");
		}
		else if(ret<4){
			printf("server close\n");
			break;
		}
		n=ntohl(recevbuf.len);
		ret=readn(sock,recevbuf.buf,n);
		if(ret==-1){
			ERR_EXIT("readn");
		}
		else if(ret<n){
			printf("server close\n");
			break;
		}
		fputs(recevbuf.buf,stdout);
		memset(&sendbuf,0,sizeof(sendbuf));
		memset(&recevbuf,0,sizeof(recevbuf));
	}
	close(sock);
	return 0;	

}
