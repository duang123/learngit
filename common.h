#ifndef _COMMON_H
#define _COMMON_H
 #include <stdio.h>
#include <errno.h>
 #include <stdlib.h>
 #include <unistd.h>

#define ERR_EXIT(m) \
{\
	perror(m);\
	exit(1);\
}\

ssize_t readn(int sock,void* buf,size_t n);
ssize_t writen(int sock,void* buf,size_t n);
ssize_t readline(int sock,void* buf,size_t maxline);

#endif
