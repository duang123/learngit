#include<string.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include<stdio.h>
#include <stdlib.h>

#define MAXNITEMS 1000000
#define MAXNTHREADS 100

int nitems;

struct {
	pthread_mutex_t mutex;
	int buff[MAXNITEMS];
	int nput;
	int nval;
} shared={
	PTHREAD_MUTEX_INITIALIZER
};

void *produce(void*),*consume(void*);

int main(int argc,char** argv){
	int i, nthreads, count[MAXNTHREADS];
	pthread_t t_produce[MAXNTHREADS],t_consume;
	if(argc!=3){
		perror("usage: prodcons2 <#item> <#threads>");
		exit(1);
	}
	nitems=atoi(argv[1]);
	nthreads=atoi(argv[2]);
	
	for(i=0;i<nthreads;++i){
		count[i]=0;
		pthread_create(&t_produce[i], NULL,produce, &count[i]);
	}
	for(i=0;i<nthreads;++i){
		pthread_join(t_produce[i],NULL);
		printf("count[%d]=%d\n",i,count[i]);
	}
	pthread_create(&t_consume,NULL,consume,NULL);
	pthread_join(t_consume,NULL);
	exit(0);
}

void* produce(void* arg){
    for(; ;){
        pthread_mutex_lock(&shared.mutex);
        if(shared.nput>=nitems){
            pthread_mutex_unlock(&shared.mutex);
            return NULL;
        }    
        shared.buff[shared.nput]=shared.nval;
        ++shared.nput;
        ++shared.nval;
        pthread_mutex_unlock(&shared.mutex);
        *((int*)arg)+=1;
    }
}

void* consume(void* arg){
    int i;
    for(i=0;i<nitems;++i){
        if(shared.buff[i]!=i){
            printf("buff[%d]=%d\n",i,shared.buff[i]);
        }
    }
}
