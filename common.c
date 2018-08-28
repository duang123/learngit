#include"common.h"
#include<string.h>
#include <sys/types.h>
#include <sys/socket.h>

ssize_t readn(int sock,void* buf,size_t n)
{
	ssize_t nread;
	size_t nleft=n;
	char* p=(char*)buf;
	while(nleft>0)
	{
		nread=read(sock,p,nleft);
		if(nread==-1){
			if(errno==EINTR)
				continue;
			return -1;
		}
		if(nread==0)
		{
			return n-nleft;
		}
		p+=nread;
		nleft-=nread;
	}
	return n-nleft;
}

ssize_t writen(int sock,void* buf,size_t n)
{
	char* ptr=(char*)buf;
	size_t nleft=n;
	ssize_t nwrite;
	while(nleft>0)
	{
		nwrite=write(sock,ptr,nleft);
		if(nwrite==-1)
		{
			if(errno==EINTR)
				continue;
			return -1;
		}
		if(nwrite==0)
			continue;
		ptr+=nwrite;
		nleft-=nwrite;
	}
	return n-nleft;
}
ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
	while(1)
	{
		char* ptr=(char*)buf;
		int ret=recv(sockfd,ptr,len,MSG_PEEK);
		if(ret==-1 && errno==EINTR)
			continue;
		return ret;
	}
}

ssize_t readline(int sock,void* buf,size_t maxline)
{
	char* ptr=(char*)buf;
	size_t nleft=maxline;
	ssize_t nread;
	int ret;
	while(1)
	{
		ret=recv_peek(sock,ptr,nleft);
		if(ret==-1)
			return -1;
		nread=ret;
		int i;
		for(i=0;i<nread;++i)
		{
			if(ptr[i]=='\n')
			{
				ret=readn(sock,ptr,i+1);
				if(ret==-1)
					return ret;
				if(ret!=i+1)
					return ret;
				return ret;
			}
		}
		if(nread>nleft)exit(1);
		ret=readn(sock,ptr,nread);
		if(ret==-1)exit(1);
		if(ret!=nread)exit(1);
		ptr+=nread;
		nleft-=nread;
	}
	return -1;
}

