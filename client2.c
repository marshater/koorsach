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
#define SENDPORT 33000
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
    struct Premessage premessage;
    char *message;
};

int main(int argc, char ** argv)
{
	struct sockaddr_in serv_addr, cli_addr;
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(SENDPORT);

	cli_addr.sin_family = PF_INET;
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    cli_addr.sin_port = htons(33002);

    struct message buff1;
    int Client_Socket,bytes_recv1;
	if ((Client_Socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
    	dieWithError("Cant accept connection");
    }
    if (connect(Client_Socket,(struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    {
    		dieWithError("Cant connect");
    };
    bytes_recv1 = recv(Client_Socket, &buff1 , sizeof(buff1), 0);







/*    if ((Client_Socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
    DieWithError("Cant accept connection");
    }
*/
}