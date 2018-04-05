#ifndef _SYSUTIL_H
#define _SYSUTIL_H
#include <sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
int read_timeout(int fd,unsigned int wait_seconds);
int write_timeout(int fd,unsigned int wait_seconds);
int connect_timeout(int fd,struct sockaddr_in* addr,unsigned int wait_seconds);
int accept_timeout(int fd,struct sockaddr_in *addr,unsigned int wait_seconds);
#endif
