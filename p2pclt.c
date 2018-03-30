#include<string.h>
#include<stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include<netinet/in.h>
#define ERR_EXIT(m)\
		{perror(m);\
		exit(1);\
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

	pid_t pid;
	pid=fork();
	if(pid==-1){
		ERR_EXIT("fork");
	}
	else if(pid==0){ 
		char recevbuf[1024]={0};
		while(1){	
			memset(recevbuf,0,sizeof(recevbuf));
			int ret=read(sock,recevbuf,sizeof(recevbuf));
			if(ret==-1){
				ERR_EXIT("read");
			}
			else if(ret==0){
				printf("server close\n");
			}
			else
				fputs(recevbuf,stdout);
		}
	}
	else { 
		char sendbuf[1024]={0}; 
		while(fgets(sendbuf,sizeof(sendbuf),stdin)!=NULL){
			write(sock,sendbuf,strlen(sendbuf));
			memset(sendbuf,0,sizeof(sendbuf));
		}
	
	}
    char recevbuf[1024]={0};
	close(sock);
	return 0;	

}
