#include"sysutil.h"
#include <sys/select.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<stdlib.h>
#include<errno.h>
#include<netinet/in.h>

//#define DEBUG
#define ERR_EXIT(m)\
		{\
			perror(m);\
			exit(1);\
		}

void activate_nonblock(int fd){
	int ret;
	int flags=fcntl(fd,F_GETFL);
	if(flags==-1)ERR_EXIT("fcntl");

	flags|=O_NONBLOCK;
	ret=fcntl(fd,F_SETFL,flags);
	if(ret==-1)ERR_EXIT("fcntl");
}

void deactivate_nonblock(int fd){
	int ret;
	int flags=fcntl(fd,F_GETFL);
	if(flags==-1)ERR_EXIT("fcntl");
	flags &= ~O_NONBLOCK;
	ret=fcntl(fd,F_SETFL,flags);
	if(ret==-1)ERR_EXIT("fcntl");
} 

int read_timeout(int fd,unsigned int wait_seconds){
	int ret=0;
	if(wait_seconds<=0){
		return ret;
	}
	else {
		fd_set readset;
		struct timeval timeout;
		
		FD_ZERO(&readset);
		FD_SET(fd,&readset);
		
		timeout.tv_sec=wait_seconds;
		timeout.tv_usec=0;
		do{
			ret=select(fd+1,&readset,NULL,NULL,&timeout);
		}while(ret<0&&errno==EINTR);
		
		if(ret==0){
			ret=-1;
			errno=ETIMEDOUT;
		}else if(ret==1){ret=0;}

	}  
	return ret;
}
/**
 * write_timeout -写超时检测函数，不含写操作
 * fd表示文件描述符
 * wait_seconds等待超时秒数，为0表示不检测超时
 *  成功（未超时）返回0,超时返回-1,且errno设置为ETIMEDOUT
 */
int write_timeout(int fd,unsigned int wait_seconds){
	int ret=0;
	if(wait_seconds<=0){
		return ret;
	}
	else{
		fd_set readset;
		struct timeval timeout;
		
		FD_ZERO(&readset);
		FD_SET(fd,&readset);

		timeout.tv_sec=wait_seconds;
		timeout.tv_usec=0;

		do{
			ret=select(fd+1,&readset,NULL,NULL,&timeout);
		}while(ret<0&&errno==EINTR);

		if(ret==0){
			ret=1;
			errno=ETIMEDOUT;
		}else if(ret==1){
			ret=0;
		}
	}
	return ret;
}

int accept_timeout(int fd,struct sockaddr_in *addr,unsigned int wait_seconds){
	int ret;
	socklen_t addrlen=sizeof(struct sockaddr_in);

	if(wait_seconds>0){
		fd_set accept_set;
		struct timeval timeout;

		FD_ZERO(&accept_set);
		FD_SET(fd,&accept_set);

		timeout.tv_sec=wait_seconds;
		timeout.tv_usec=0;

		do{
			ret=select(fd+1,&accept_set,NULL,NULL,&timeout);
		}while(ret<0&&errno==EINTR);

		if(ret==0){
			ret=1;
			errno=ETIMEDOUT;
		}else if(ret==-1)return -1;

	}
	if(addr!=NULL){
		ret=accept(fd,(struct sockaddr*)addr,&addrlen);
	}else ret=accept(fd,NULL,NULL);

	if(ret==-1){
		ERR_EXIT("accept");
	}
	return ret;	
}

int connect_timeout(int fd,struct sockaddr_in* addr,unsigned int wait_seconds){
	if(wait_seconds>0)//若wait_seconds大于0,先设置文件描述为非阻塞，这样调用connect函数失败的话，可以立即返回
		activate_nonblock(fd);
	int ret;
	ret=connect(fd,(struct sockaddr*)addr,sizeof(struct sockaddr_in));
	if(ret==-1 && errno==EINPROGRESS){
#ifdef DEBUG
		printf("AAA\n");
#endif
		fd_set connect_set;
		struct timeval timeout;
		FD_ZERO(&connect_set);
		FD_SET(fd,&connect_set);
		timeout.tv_sec=wait_seconds;
		timeout.tv_usec=0;

		do{
			ret=select(fd+1,NULL,&connect_set,NULL,&timeout);
		}while(ret<0 && errno==EINTR);

		if(ret==0)
		{
			ret= -1;
			errno=ETIMEDOUT;
		}else if(ret<0)
			return -1;
		else if(ret==1)
		{
#ifdef DEBUG
			printf("BBB\n");
#endif
			int err;
			socklen_t socklen=sizeof(err);
			int sockoptret=getsockopt(fd,SOL_SOCKET,SO_ERROR,&err,&socklen);
			if(sockoptret==-1){
				return -1;
			}
			if(err==0)
			{
				ret=0;
			}else
			{
#ifdef DEBUG
				printf("CCC\n");
#endif
				errno=err;
				ret=-1;
			}
		}
	}
	if(wait_seconds>0){
		deactivate_nonblock(fd);
	}
	return ret;

}

