 #include <stdio.h>
 #include <stdlib.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <pthread.h>
 #include <string.h>
 #define PORT 8795
 #define SERVER_ADDR "127.0.0.1"
 
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
 	int socketfd,sock_bk,k;
 	pthread_t thread_send,thread_read;
 	struct sockaddr_in svaddr;
 	socketfd=socket(AF_INET,SOCK_STREAM,0);
	if (socketfd==-1) printf("can't create a socket!");
 	bzero(&svaddr,sizeof(svaddr));
 	svaddr.sin_port=htons(PORT);
 	svaddr.sin_family=AF_INET;
 	svaddr.sin_addr.s_addr=inet_addr(SERVER_ADDR); //convert string to server address 32bit
 	if (connect(socketfd,(struct sockaddr *)&svaddr,sizeof(svaddr))<0)
	{
		printf("Can not connect!");
		exit(0);
	}
	sock_bk=socketfd;
		pthread_create(&thread_send,NULL,&sendmg,&sock_bk);
		readmg(socketfd);
	return 0;
 }
