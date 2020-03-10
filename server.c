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

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

int acceptConnection(int Server_Socket)
{
    int Client_Socket;
    struct sockaddr_in Client_Address;
    unsigned int Client_Length;

    if(Client_Socket = accept(Server_Socket, (struct sockaddr *) &Client_Address, sizeof(Client_Address)) < 0)
    {
        DieWithError("Cant accept connection");
    }
    printf("Accepted with", inet_ntoa(Client_Address.sin_addr));
    return(Client_Socket);
}

int binder (int Server_Socket)
{
    if(Bind_Socket = bind(Server_Socket, (struct sockaddr *) &Client_Address, sizeof(Client_Address)) < 0)
    {
    DieWithError("Cant accept connection");
    }
}

int Socketer(NULL)
{
    int Req_Socket
    if ((Req_Socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    {
    DieWithError("Cant accept connection");
    }
    return Req_Socket
}

int main(int argc, char ** argv)
{
    char * server_ip = argv[3];
    char * server_port = argv[4];

    server_addr.sin_family = PF_NET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    UDP_SOCKET_1 = Socketer();

}
