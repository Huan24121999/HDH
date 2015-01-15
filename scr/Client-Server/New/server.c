#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#define PORT 8795
#define BACKLOG 5
#define MAXTHREAD 3
#define MAX_BUFFER_SIZE 255

int server_fd, newsock_fd, client_fd[MAXTHREAD], IN, OUT, conv_fd, client_addr_len;
struct sockaddr_in client_addr[MAXTHREAD];
pthread_t TID[MAXTHREAD], PTID, CTID, WTID;
sem_t full, empty, connection;
pthread_mutex_t mutex_client_fd = PTHREAD_MUTEX_INITIALIZER, mutex_in = PTHREAD_MUTEX_INITIALIZER, mutex_out = PTHREAD_MUTEX_INITIALIZER;

void* read_msg(void*);
void* write_msg(void*);
void* producer(void* unused);
void* consumer(void* unused);

 int main(void)
{	printf("+------------------------------+\n");
	printf("+         CHAT SERVER          +\n");
	printf("+        PHAM VAN THONG        +\n");
	printf("+------------------------------+\n");
	printf("   --- version 1.0.0 ---\n");
	struct sockaddr_in server_addr,client_addr;
	sem_init(&full, 0, 0);
	sem_init(&empty, 0, MAXTHREAD);
	sem_init(&connection, 0, 0);
	server_fd = socket(AF_INET, SOCK_STREAM, 0); //man 7 ip
	if (server_fd == -1)
	{
		printf("\nCan't create socket!");
		exit(1);
	}
	
	server_addr.sin_family = AF_INET; 
	server_addr.sin_port = htons(PORT); //Doi sang network byte order
	server_addr.sin_addr.s_addr = INADDR_ANY; //Nhan bat cu ket noi tu IP nao

	if (!bind(server_fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in)) == -1)
	{
		printf("\nCan't bind!");
		exit(2);
	}

	listen(server_fd, BACKLOG);
	pthread_create(&WTID, NULL, &write_msg, NULL); //luong gui tin nhan
	pthread_create(&PTID,NULL,&producer,NULL); //luong chap nhan ket noi accept
	pthread_create(&CTID, NULL, &consumer, NULL); //luong tao giao tiep
	pthread_join(PTID, NULL);
	pthread_join(CTID, NULL);
	return 0;
}

//Dua ket noi vao hang doi
void* producer(void* unused)
{
	while (1)
	{
		sem_wait(&empty);
		client_fd[IN] = accept(server_fd, (struct sockaddr*) &client_addr[IN], &client_addr_len);
		IN = (IN + 1) % MAXTHREAD;
		sem_post(&full);
	}
	return NULL;
}
//Lay thong tin ket noi tu hang doi tao luong giao tiep
void* consumer(void* unused)
{
	while (1)
	{
		sem_wait(&full);
		pthread_mutex_lock(&mutex_out);
		pthread_create(&TID[OUT],NULL,&read_msg,(void*)&client_fd[OUT]);
		printf("\nConnect to client %d \n",client_fd[OUT]);
		OUT = (OUT + 1) % MAXTHREAD;
		pthread_mutex_unlock(&mutex_out);
	}
}
//doc tin tu socket fd
void* read_msg(void *socketfd)
{
	int n = *((int*) socketfd);
	char notice[]="Connect to server success!";
	write(*((int*)socketfd), (void*) notice, strlen(notice));
	while (1)
	{
		char buff[MAX_BUFFER_SIZE];
		bzero(buff,MAX_BUFFER_SIZE);
		read(n, (void*) buff, MAX_BUFFER_SIZE);
       		if (!strcmp(buff,"exit") || !strcmp(buff,""))
			break;
		printf("\nReceive from client %d\n--> \"%s\"\n",n,buff);
	}
	printf("\n!!!Client %d disconnect!!!\n",n);
	close(n);
	sem_post(&empty);
	return NULL;
}

//gui tin, dich den la socketfd duoc nhap tu ban phim
//co dang: "socketfd< text message"
void* write_msg(void* unused)
{
	int socketfd, i;
	while (1)
	{
		char buff[MAX_BUFFER_SIZE];
		bzero(buff,MAX_BUFFER_SIZE);
		fgets(buff, MAX_BUFFER_SIZE, stdin);
		buff[strlen(buff)-1]=0;
		socketfd=atoi(buff);
		i=0;
		while (buff[i] !='<') i++;
      		write(socketfd, (void*) buff+i+1, strlen(buff));
	}
}

