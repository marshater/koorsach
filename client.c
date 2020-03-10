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

char *MessageGenerator(int N)
{
	char *pack;
	srand(time(NULL));
	static char charset[] = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890";
	int lenght = N;
	pack = malloc(sizeof(char) * lenght);
	for(int i = 0; i < lenght; i++ )
	{
		pack[i] = charset[rand()%(strlen(charset))];
	}
	return (char*)pack;
}

struct Premessage FillPremessage(struct Premessage Premessage) //Больше премессагов богу премессагов! :)
{
	srand(time(NULL));
	Premessage.TimeToSleep = rand()%8;
	Premessage.Number = rand()%30;
	return(Premessage);
}

int main(int argc, char ** argv)
{
	int UDP_SOCKET;
    char * server_ip = argv[1];
    char * server_port = argv[2];
    char * client_port = argv[3];

    struct sockaddr_in server_addr = {0};
    struct sockaddr_in client_addr = {0};

    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(client_port);

    int N = 3;
	char *message;


    if ((UDP_SOCKET = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
    dieWithError("Cant accept connection");
    }

    int rc = bind(UDP_SOCKET, (struct sockaddr *) &client_addr, sizeof(client_addr));

    if(rc == -1)
    {
        dieWithError("Cant bind");
        close(UDP_SOCKET);
    }

	struct Premessage name;
	name = FillPremessage(name);
	printf("%i\n", name.TimeToSleep);
	printf("%i\n", name.Number);
	message = MessageGenerator(name.Number);

	for (int i = 0; i < name.Number; i++)
	{
	printf("%c\n", message[i]);
	}
	rc = sendto(rc, &message, sizeof(*message), 0, (struct sockaddr*) &server_addr, sizeof(struct sockaddr_in));


}