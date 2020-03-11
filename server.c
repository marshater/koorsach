#include "TCPEchoServer.h"  /* TCP echo server includes */
#include <sys/time.h>       /* for struct timeval {} */
#include <fcntl.h>          /* for fcntl() */
#include <stdio.h>      /* for printf() */
#include <sys/socket.h> /* for accept() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdio.h>  /* for perror() */
#include <stdlib.h> /* for exit() */
#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define RCVBUFSIZE 32   /* Size of receive buffer */
#define RECEIVEPORT 32000
#define MAXLENGHT 30

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

void ReceiveTCP(void *arg)
{
	struct sockaddr_in serv_addr, cli_addr;
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(RECEIVEPORT);

	cli_addr.sin_family = PF_INET;
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    cli_addr.sin_port = htons(32001);

	socklen_t clilen;
	clilen = sizeof(cli_addr);

    int Client_Socket, Bind_Socket, bytes_recv1, bytes_recv2;
    char buff2[MAXLENGHT];
    int *buff1;


    if ((Client_Socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
    DieWithError("Cant accept connection");
    }
    if(bind(Client_Socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
    DieWithError("Cant accept connection");
    }
    listen(Client_Socket, 5);
    if(Bind_Socket = accept(Client_Socket, (struct sockaddr *) &cli_addr, &clilen) < 0)
    {
        DieWithError("Cant accept connection");
    }
    //printf("Accepted with ", inet_ntoa(cli_addr.sin_addr),"\n");
    bytes_recv1 = recv(Bind_Socket, &buff1 , sizeof(buff1), 0);
    bytes_recv2 = recv(Bind_Socket,&buff2, MAXLENGHT, 0);
    printf("%i\n", *buff1);
    printf("%c\n", (char)buff2[0]);
}

/*int UDP_Broadcast()
{
	if ((Client_Socket = socket(PF_INET, SOCK_STREAM, SO_REUSEADDR)) < 0)
    DieWithError("Cant accept connection");
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

}
*/
int main(int argc, char ** argv)
{
	ReceiveTCP(0);
}
