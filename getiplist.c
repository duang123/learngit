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
#include <netdb.h>

#define ERR_EXIT(m)\
		{perror(m);\
		exit(1);\
		}

int main(){
	char host[100]={0};
	size_t len=sizeof(host);

	if(gethostname(host,len)<0){
		ERR_EXIT("gethostname");
	}
	//struct hostent *gethostbyname(const char *name);
	struct hostent* hostlist;
	if((hostlist=gethostbyname(host))==NULL){
		ERR_EXIT("gethostbyname");
	}
	int i=0;
	while(hostlist->h_addr_list[i]!=NULL){
		printf("%s\n",inet_ntoa(*(struct in_addr*)(hostlist->h_addr_list[i])));
		++i;
	}

}
