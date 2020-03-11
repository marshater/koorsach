#include "TCPEchoServer.h"  /* TCP echo server includes */
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

#define MAX_LENGHT 30
#define MAX_SLEEPTIME 10
#define RECEIVEPORT 32000

void dieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

struct Premessage
{
	int TimeToSleep;
	int Number;
};

struct message
{
    struct sockaddr_in info;
    struct Premessage premessage;
    char *message;
};

char *MessageGenerator(int N)
{
    char *pack;
    srand(time(NULL));
    static char charset[] = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890";
    int lenght = N;
    pack = malloc(sizeof(char) * lenght);
    for(int i = 0; i < lenght; i++)
    {
        pack[i] = charset[rand()%(strlen(charset))];
    }
    return (char*)pack;
}

struct Premessage FillPremessage(struct Premessage Premessage) //Больше премессагов богу премессагов! :)
{
    srand(time(NULL));
    Premessage.TimeToSleep = rand()%MAX_SLEEPTIME;
    Premessage.Number = rand()%MAX_LENGHT + 1;
    return(Premessage);
}

int SendTCP(void* arg)
{
    struct sockaddr_in client_addr = {0};
    struct sockaddr_in server_addr = {0};
    client_addr.sin_family = PF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(32001);

    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(RECEIVEPORT);

    struct Premessage name;
    name = FillPremessage(name);
    char *mess = MessageGenerator(name.Number);
/*struct message *info = (struct message*) arg;
struct sockaddr_in serv_adr = info->info;
struct Premessage premessage = info->premessage;
char *msg = info->message;*/
    int Client_Socket;
//    printf(stderr, "123\n");

    printf("%i\n", name.TimeToSleep);


	if ((Client_Socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    	{
    	dieWithError("Cant accept connection");
    	}
    if (connect(Client_Socket,(struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
    	{
    		dieWithError("Cant connect");
    	};
    printf("%i\n", &name);
    send(Client_Socket,&name, sizeof(name), 0);
    send(Client_Socket, &mess, sizeof(mess), 0);
}


int main(int argc, char ** argv)
{
	int UDP_SOCKET;

    struct sockaddr_in server_addr = {0};
    struct sockaddr_in client_addr = {0};
/*    struct Premessage name;
    name = FillPremessage(name);
    char *mess = MessageGenerator(name.Number);*/



/*    if ((TCP_SOCKET = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
    dieWithError("Cant accept connection");
    }

    int rc = bind(TCP_SOCKET, (struct sockaddr *) &client_addr, sizeof(client_addr));

    if(rc == -1)
    {
        dieWithError("Cant bind");
        close(UDP_SOCKET);
    }

*/



    SendTCP(0);
}