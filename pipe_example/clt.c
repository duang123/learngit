#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<string.h>
#include<unistd.h>
#define ERR_EXIT(m)\
{\
        perror(m);\
        exit(1); \
}\

static int sockfd;
static FILE* fp;
void str_cli(FILE* fp_arg,int sock_arg,struct sockaddr* servaddr,socklen_t servlen)
{
        char sendline[1024]="uqwerrtiop[asdfggh\n";
        sockfd=sock_arg;
        fp=fp_arg;
        socklen_t len=sizeof(struct sockaddr_in);
//      memset(sendline,0,sizeof(sendline));
//      while(fgets(sendline,1024,fp)!=NULL)
        while(1)
       {
                sendto(sockfd,sendline,strlen(sendline),0,servaddr,sizeof(*servaddr));
        //        memset(sendline,0,sizeof(sendline));
        }

}

int main()
{
        struct sockaddr_in servaddr;
        int sockfd;
        sockfd=socket(AF_INET,SOCK_DGRAM,0);
        if(sockfd==-1)
                ERR_EXIT("socket");
        memset(&servaddr,0,sizeof(servaddr));
        servaddr.sin_family=AF_INET;
        servaddr.sin_port=2525;
        servaddr.sin_addr.s_addr=inet_addr("10.239.82.215");
        str_cli(stdin,sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
        close(sockfd);
        return 0;
}
