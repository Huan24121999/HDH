
 #include <stdio.h>
 #include <stdlib.h>
 #include <sys/types.h>
 #include <sys/socket.h>
#include <netinet/in.h>
 #include <pthread.h>
 #include <string.h>
#define PORT 8795

/* send messages to a socket */
 void* sendmg(void* socket)
{
 	char outmsg[256];
 	int *p,socketfd;
 	p=(int*)socket;
	socketfd=*p;
	while (1)
	{
		bzero(outmsg,256);
		printf("\nOut msg: ");
		fgets(outmsg,255,stdin);
		if (strcmp(outmsg,"exit")==0) break;
		write(socketfd,&outmsg,sizeof(outmsg));
	}
	return (NULL);
}
 
 /* Read message from socket and print on screen */
 void readmg(int socketfd)
 {
	char inmsg[256];
	while (1)
	{
	  bzero(inmsg,256);
		read(socketfd,&inmsg,255);
 		printf("\nIn msg: %s",inmsg);
 	}
 }
 
 int main(void)
{
 	int sockfd,newsock,clientlen,sock_bk,k=0; /* socket file describes */
 	pthread_t thread_send,thread_read;
 	struct sockaddr_in svaddr,claddr;
 	sockfd=socket(AF_INET,SOCK_STREAM,0); /* initialize socket */
 	/*
 	 * AF_INET: address family. ex: AF_UNIX, AF_OSI,...
 	 * SOCK_STREAM: type of socket ex: SOCK_DGRAM (Datagram);
 	 * 0: auto protocol. ex: TCP, UDP
 	 */
	if (sockfd==-1)
		printf("can't initialize a socket!"),exit(0);
	bzero((void*)&svaddr,sizeof(svaddr));
	svaddr.sin_family=AF_INET;
	svaddr.sin_addr.s_addr=INADDR_ANY;
	/*
	Address to accept any incoming messages.
 	= inet_addr("192.168.1.1") if have client address.
 	 */
	svaddr.sin_port=htons(PORT);
	if (bind(sockfd,(struct sockaddr*)&svaddr,sizeof(svaddr))==-1)
		printf("can't not bind!"),exit(0);
	listen(sockfd,100);
	newsock=accept(sockfd,(struct sockaddr *)&claddr,&clientlen);
	if (newsock==-1) printf("ERROR to accept connection");
	else
		printf("accept request from client\n");
	sock_bk=newsock;
	pthread_create(&thread_send,NULL,&sendmg,&sock_bk);
	readmg(newsock);
        return 0;
}
