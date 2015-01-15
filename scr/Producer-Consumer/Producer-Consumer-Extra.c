#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MaxN 5

pthread_mutex_t mutex_in = PTHREAD_MUTEX_INITIALIZER,mutex_out = PTHREAD_MUTEX_INITIALIZER;
sem_t empty, full;
/* empty: So phan tu co the them vao hang doi
   full: So phan tu trong hang doi */
int buff[MaxN], IN,OUT;
/*IN: queue head - OUT: queue rear */

void* producer(void* unused);
void* consumer(void* unused);
int fibonaci(int n);

int main(void)
{
	pthread_t TID1,TID2,TID3,TID4,TID5;
	sem_init(&empty,0,MaxN);
	sem_init(&full,0,0);
	pthread_create(&TID1,NULL,&producer,NULL);
	pthread_create(&TID2,NULL,&consumer,NULL);
	//pthread_create(&TID3,NULL,&consumer,NULL);
	//pthread_create(&TID4,NULL,&consumer,NULL);
	//pthread_create(&TID5,NULL,&consumer,NULL);
	pthread_join(TID1,NULL);
	//pthread_join(TID2,NULL);
	//pthread_join(TID3,NULL);
	//pthread_join(TID4,NULL);
	//pthread_join(TID5,NULL);
	return 0;
}

int fibonacci(int n)
{
	if (n==0 || n==1)
		return 1;
	return fibonacci(n-1)+fibonacci(n-2);
}

void* producer(void* unused)
{
	int n;
	while (1)
	{
		printf("\nEnter n: ");
		scanf("%d",&n);
		sem_wait(&empty); //Neu buff day thi cho doi
		//pthread_mutex_lock(&mutex_in); //Truong hop nhieu luong nhap
		buff[IN]=n;
		IN=(IN+1)%MaxN;
		sem_post(&full);
		//pthread_mutex_unlock(&mutex_in);
	}
	return NULL;
}


void* consumer(void* unused)
{
	int n,fb;
	while (1)
	{
		sem_wait(&full); //Neu buff rong thi doi
		//pthread_mutex_lock(&mutex_out); //Truong hop nhieu luong xu ly
		n=buff[OUT];
		OUT=(OUT+1)%MaxN;
		sem_post(&empty);
		//pthread_mutex_unlock(&mutex_out);
		fb=fibonacci(n);
		printf("\nfibonacci(%d): %d  ",n,fb);
	
	}
	return NULL;
}
