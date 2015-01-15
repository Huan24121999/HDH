#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MaxN 5
int BUF[MaxN], N, IN, OUT, A;

unsigned int fibonacci(int n)
{
	if (n == 0 || n == 1) 
		return 1;
	return fibonacci(n - 1) + fibonacci(n - 2);
}


void* producer(void *unused)
{
	while (1)
	{
		while (N == MaxN); //Neu bo dem day thi doi
		printf("Enter n: ");
		scanf("%d", &BUF[IN]);
		IN=(IN+1) % MaxN;
		N=N+1;
	}
}

void* consumer(void* unused)
{
	int n;
	while (1)
	{
		while (N == 0); //Neu bo dem rong thi doi
		n = BUF[OUT];
		OUT = (OUT + 1) % MaxN;
		N = N-1;
		printf("\nfibonacci(%d): %u   ", n, fibonacci(n));
	}
}

int main(void)
{
	pthread_t TID1,TID2;
	pthread_create(&TID1,NULL,&producer,NULL);
	//Thread ID, thread attribute, pointer to function, argument
	pthread_create(&TID2,NULL,&consumer,NULL);
	pthread_join(TID1,NULL);
	pthread_join(TID2,NULL);
	return 0;
}
