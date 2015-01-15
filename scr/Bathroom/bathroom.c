#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <pthread.h>

#define AMOUNT 100 //So luong nguoi
#define MAN 5 //So luong boy toi da trong phong tam
#define WOMAN 5 //So luong girl toi da trong phong tam
#define AVGTIME 10 //thoi gian su dung trung binh
#define STDDEVTIME 100 //Do lech chuan thoi gian tam

pthread_t maleTID, femaleTID, makethreadTID1,makethreadTID2, makethreadTID3;
sem_t male_count, female_count, empty;
pthread_mutex_t mutex_male = PTHREAD_MUTEX_INITIALIZER, mutex_female = PTHREAD_MUTEX_INITIALIZER,
	mutex_thread=PTHREAD_MUTEX_INITIALIZER;
int mcount, fcount, mtime, ftime, muse, fuse;

unsigned int delay_time(unsigned int delaytime, unsigned int stddeviation);
//input in miliseconds
void* makethread(void*);
void* male_routine(void* unused);
void* female_routine(void* unused);
int freq();

int main(void)
{
	int i=0;
	sem_init(&male_count, 0, MAN);
	sem_init(&female_count, 0, WOMAN);
	sem_init(&empty, 0, 1); 

	pthread_create(&makethreadTID1, NULL, &makethread, NULL);
	pthread_create(&makethreadTID2, NULL, &makethread, NULL);
	pthread_create(&makethreadTID3, NULL, &makethread, NULL);
	sleep(10);
	printf("\n\n+-------------------------------+");
	printf("\n+     %5d     |     %5d     +",mtime, ftime);
	printf("\n+      MAN      |     WOMAN     +");
	printf("\n+-------------------------------+\n\n");
	return 0;
}

void* makethread(void* unused)
{
	int n;
	static int i=0;
	while (i < AMOUNT)
	{
		delay_time(0, 500);
		n=freq();
		if (n == 0)
			pthread_create(&maleTID, NULL, &male_routine, NULL);
		else
			pthread_create(&femaleTID, NULL, &female_routine, NULL);
		pthread_mutex_lock(&mutex_thread);
		i++;
		pthread_mutex_unlock(&mutex_thread);
	}
	return NULL;
}

void* male_routine(void* unused)
{
	pthread_mutex_lock(&mutex_male); //avoid race condition
	mcount++; //tang so luong nguoi dang cho
	if (mcount ==1) //Neu chua co boy nao
		sem_wait(&empty); //thi cho cho girl ra het
	//else da co boy trong phong
	pthread_mutex_unlock(&mutex_male);

	sem_wait(&male_count); //Cho cho den khi con cho trong
	printf("\n---> Man entered");
	delay_time(AVGTIME, STDDEVTIME);
	printf("\nMan exited --->");
	sem_post(&male_count); //Tang so luong cho trong

	pthread_mutex_lock(&mutex_male); 
	mcount--; //Giam so luong boy trong phong
	if (mcount == 0) //Neu khong con boy nao
	{
		printf("\n\n\"Bathroom empty!\"\n");
		printf("Man used time: %d\n",muse+1);
		muse=0;
		sem_post(&empty); //Thong bao phong khong co ai
	}
	else
		muse++;
	mtime++;
	pthread_mutex_unlock(&mutex_male);
	return NULL;
}

void* female_routine(void* unused)
{
	pthread_mutex_lock(&mutex_female);
	fcount++;
	if (fcount == 1)
		sem_wait(&empty);
	pthread_mutex_unlock(&mutex_female);

	sem_wait(&female_count);
	printf("\n---> Woman entered");
	delay_time(AVGTIME, STDDEVTIME);
	printf("\nWoman exited --->");
	sem_post(&female_count);

	pthread_mutex_lock(&mutex_female);
	fcount --;
	if (fcount ==0)
	{
		printf("\n\n\"Bathroom empty!\"\n");
		printf("Woman used time: %d\n",fuse+1);
		fuse=0;
		sem_post(&empty);
	}
	else
		fuse++;
	ftime++;
	pthread_mutex_unlock(&mutex_female);
	return NULL;
}

int freq()
{
	int n;
	srand(time(NULL));
	n = rand() % 100;
	if (n<50)
		return 1;
	return 0;
}


unsigned int delay_time(unsigned int delaytime, unsigned int stddeviation) 
{
	unsigned int n;
	if (stddeviation == 0)
		stddeviation = 300;
	srand(time(NULL));
	n = delaytime * 1000 + rand() % (stddeviation*1000);
	usleep(n);
	return n;
}
