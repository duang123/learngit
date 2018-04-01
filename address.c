#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<stdio.h>
#include<string.h>

int main(){

	in_addr_t addr1=inet_addr("192.2.2.0");
	printf("addr1=%d\n",ntohl(addr1));
	struct in_addr str_addr;
	str_addr.s_addr=addr1;
	printf("str_addr的地址是:%s\n",inet_ntoa(str_addr));
	return 0;
}

