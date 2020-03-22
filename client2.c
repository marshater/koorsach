#include <unistd.h>
#include <sys/time.h>       /* for struct timeval {} */
#include <fcntl.h>          /* for fcntl() */
#include <stdio.h>      /* for printf() */
#include <sys/socket.h> /* for accept() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdio.h>  /* for perror() */
#include <stdlib.h> /* for exit() */
#include <netinet/in.h>
#include <netinet/in.h>
#include <time.h>
#include <pthread.h>
#define SENDPORT 33000
#define MAXMSGLEN 30
#define RESPORT 10
#define TCPPORT 35000
#define UDPPORT 17000

pthread_mutex_t status = PTHREAD_MUTEX_INITIALIZER;

struct message
{
    int TimeToSleep;
    int Number;
    char message[MAXMSGLEN];
};


void dieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

void *TCP_listen(void *arg)
{
	struct sockaddr_in serv_addr, cli_addr;
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(SENDPORT);

	cli_addr.sin_family = PF_INET;
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    cli_addr.sin_port = htons(TCPPORT);

    srand(time(NULL));
	int buff1, Client_Socket;


	if ((Client_Socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
        dieWithError("cant socket");
	}

    while (connect(Client_Socket,(struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    {
		cli_addr.sin_port = htons(TCPPORT+1+rand()%RESPORT);
    };

    printf("loh3\n");

	recv(Client_Socket, &buff1, sizeof(int), 0);

	struct message
{
    int TimeToSleep;
    int Number;
    char message[buff1];
};

	struct message buff;

	recv(Client_Socket, &buff , sizeof(buff), 0);

	printf("%i\n", buff.Number);
    printf("%i\n", buff.TimeToSleep);

   for(int i = 0; i < buff.Number; i++)
   {
    	printf("%c", buff.message[i]);
   }
    sleep(buff.TimeToSleep);
}

void *UDP_listener(void *arg)
{
	pthread_t TCP;
    int checker1, Client_Socket;
	struct sockaddr_in client_addr = {0};
    struct sockaddr_in server_addr = {0};
    srand(time(NULL));

	server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(UDPPORT);


    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(UDPPORT+1);

    if ((Client_Socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
    	dieWithError("Cant bind connection");
    }



    while (bind(Client_Socket, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0)
    {
    client_addr.sin_port = htons(UDPPORT+rand()%RESPORT);
	}

    int client_addr_size = sizeof(struct sockaddr_in);
    while(1)
    {
    	pthread_mutex_lock(&status);
    	recvfrom(Client_Socket, &checker1, sizeof(int), 0, (struct sockaddr*) &server_addr, &client_addr_size);
    	
    	if (checker1 == 1) 
    	{
        	pthread_create(&TCP, NULL, TCP_listen, NULL);
        	pthread_join(TCP, NULL);
        	pthread_mutex_unlock(&status);
//}
		}
	}
	close(Client_Socket);
}

int main(int argc, char * argv[])
{
	pthread_t UDP;

	pthread_create(&UDP, NULL, UDP_listener, NULL);
	pthread_join(UDP, NULL);
	return 0;
}