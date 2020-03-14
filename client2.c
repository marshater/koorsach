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
#define SENDPORT 33000
#define MAXMSGLEN 30

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



int main(int argc, char ** argv)
{
	struct sockaddr_in serv_addr, cli_addr;
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(SENDPORT);

	cli_addr.sin_family = PF_INET;
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    cli_addr.sin_port = htons(33001);

	struct message buff;
    int bytes_recv1, bytes_recv2;;

    int Client_Socket;
	if ((Client_Socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
    	dieWithError("Cant accept connection");
    }
    if (connect(Client_Socket,(struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    {
    		dieWithError("Cant connect");
    };

    //bytes_recv2 = recv(Client_Socket, buff1, sizeof(buff1), 0);
    bytes_recv1 = recv(Client_Socket, &buff , MAXMSGLEN, 0);

	printf("%i\n", buff.Number);
    printf("%i\n", buff.TimeToSleep);
    for(int i = 0; i < buff.Number; i++)
    	printf("%c", buff.message[i]);


return 0;







/*    if ((Client_Socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
    DieWithError("Cant accept connection");
    }
*/
}