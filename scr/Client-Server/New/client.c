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

#define PORT 8795
#define MAXTHREAD 3
#define MAX_BUFFER_SIZE 255

void* read_msg(void* socket_fd);
void* write_msg(void* socket_fd);

int main(int argv, char **argc)
{
	printf("+------------------------------+\n");
	printf("+         CHAT CLIENT          +\n");
	printf("+        PHAM VAN THONG        +\n");
	printf("+------------------------------+\n\n");
	int client_fd, server_addr_len;
	pthread_t WTID, RTID;
	struct sockaddr_in server_addr;
	char SERVER_ADDR[16] = "127.0.0.1";
	if (argc[1])
		strcpy(SERVER_ADDR,argc[1]);

	client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd == -1)
	{
		printf("\nCan't create socket!");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR); 
//Chuyen doi dia chi sang dang nhi phan kieu network byte order. 

	if (connect(client_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1)
	{
		printf("\nCan't connect to server\n");
		exit(2);
	}
	pthread_create(&WTID, NULL, &write_msg, &client_fd);
	pthread_create(&RTID, NULL, &read_msg, &client_fd);
	pthread_join(WTID, NULL);
	pthread_join(RTID, NULL);
}


void* read_msg(void *socketfd)
{
	while (1)
	{
		char buff[MAX_BUFFER_SIZE];
		bzero(buff,MAX_BUFFER_SIZE);
		read(*((int*)socketfd), (void*) buff, MAX_BUFFER_SIZE);
       		if (!strcmp(buff,"exit") || !strcmp(buff,""))
			break;
		printf("\nReceive from server \n--> \"%s\"\n",buff);
	}
	exit(1);
}

void* write_msg(void* socket_fd)
{
	while (1)
	{
		char buff[MAX_BUFFER_SIZE];
		bzero(buff,MAX_BUFFER_SIZE);
		fgets(buff,MAX_BUFFER_SIZE,stdin);
		buff[strlen(buff)-1] = 0;
		write(*((int*) socket_fd), (void*) buff, strlen(buff));
		if (!strcmp(buff,"exit")|| !strcmp(buff,""))
		{
			printf("\nDisconnect from server\n");
			exit(2);
		}
	}
}

/*void* read_msg(void* socket_fd)
{
	while (1)
	{
		char buff[MAX_BUFFER_SIZE];
		bzero(buff,MAX_BUFFER_SIZE);
		read(*((int*) socket_fd), (void*) buff, MAX_BUFFER_SIZE);
		if (!strcmp(buff,"exit") || !strcmp(buff,""))
			break;
		printf("\nReceive from server %d\n --> \"%s\"",1,buff);
	}
	exit(1);
}*/

