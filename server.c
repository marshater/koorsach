#include <unistd.h>
#include <sys/time.h>       /* for struct timeval {} */
#include <fcntl.h>          /* for fcntl() */
#include <stdio.h>      /* for printf() */
#include <sys/socket.h> /* for accept() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdio.h>  /* for perror() */
#include <stdlib.h> /* for exit() */
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <errno.h>


#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define RCVBUFSIZE 32   /* Size of receive buffer */
#define RECEIVEPORT 32000
#define SENDPORT 33000
#define MAXMSGLEN 30
#define MSGMAX 10

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}
struct msgbuf 
{
    long mtype;   /* тип сообщения, должен быть > 0 */
    char mtext[1];        /* содержание сообщения */
}; 

void *ReceiveTCP(void *arg);
void *SendTCP(void *arg);

/*int queueMaster(int Newqueue,char* msg)
{
	struct msgbuf msghold;
	char *ptr;
	int lenght;
	if(msgsnd(Newqueue, *ptr, sizeof(struct msgbuf)-sizeof(long), 0, IPC_NOWAIT) == -1)
		DieWithError("Cant create queue");
}
*/
/*int UDP_Broadcast2(void* arg)
{
	int UDP_Socket;
	int Newqueue;
	char *message;
	char *buf;
	struct sockaddr_in serv_addr, cli_addr;
	cli_addr.sin_family = PF_INET;
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    cli_addr.sin_port = htons(32001);
	if ((UDP_Socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    DieWithError("Cant accept connection");
	*message = msgrcv(Newqueue, &buf,sizeof(struct msgbuf)-sizeof(long),0,0);
	sendto(UDP_Socket, &message, sizeof(message), 0, (struct sockaddr*) &cli_addr, sizeof(struct sockaddr_in));
}
int UDP_Broadcast1(void* arg)
{

}
*/
int main(int argc, char ** argv)
{	
	pthread_t TCP1, TCP2, UDP1, UDP2;
	key_t key = IPC_PRIVATE;
	int Newqueue = msgget(key, IPC_CREAT);
	if (Newqueue < 0)
		DieWithError("Cant create queue");
	printf("bruh0");

	int result = pthread_create(&TCP1, NULL, ReceiveTCP, &Newqueue);
		if (result != 0) {
		perror("Creating the first thread");
		return EXIT_FAILURE;
	}
	pthread_join(TCP1, NULL);
//	pthread_create(&TCP2, NULL, SendTCP, &Newqueue);
		msgctl(result, IPC_RMID, NULL);
}

void *ReceiveTCP(void *arg) //иногда трассируется до пятого, чаще всего остается на нуле.
{
	struct sockaddr_in serv_addr, cli_addr;
	int Client_Socket, Bind_Socket, bytes_recv1, bytes_recv2;
    char *buff2;
    int *buff1;
    int Newq; //=(int) arg;
    printf("%i\n",Newq);
    printf("bruh1");
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(RECEIVEPORT);

	cli_addr.sin_family = PF_INET;
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    cli_addr.sin_port = htons(32001);

	socklen_t clilen;
	clilen = sizeof(cli_addr);


	printf("bruh2");
    Client_Socket = socket(PF_INET, SOCK_STREAM, 0);

    if(Client_Socket < 0)
    {
    DieWithError("Cant accept connection");
    }

    printf("bruh3");
	bind(Client_Socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if(Bind_Socket < 0)
    {
    DieWithError("Cant accept connection");
    }

    printf("bruh4");
    listen(Client_Socket, 5);

    while(1){
    printf("bruh5");
	Bind_Socket = accept(Client_Socket, (struct sockaddr *) &cli_addr, &clilen);
    if(Bind_Socket < 0)
    {
        DieWithError("Cant accept connection");
    }
    printf("Accepted with ", inet_ntoa(cli_addr.sin_addr),"\n");
    bytes_recv1 = recv(Bind_Socket, &buff1 , sizeof(buff1), 0);
    bytes_recv2 = recv(Bind_Socket, &buff2, sizeof(buff1), 0);
    printf("bruh6");
    printf("%i\n", *buff1);
    printf("%c\n", (char)buff2[0]);
    int test = msgsnd(Newq, &buff2, sizeof(struct msgbuf)-sizeof(long), 0);
	if(test == -1)
		DieWithError("Cant send to queue");
}
close(Client_Socket);
close(Bind_Socket);
pthread_exit(0);
}

/*void *SendTCP(void *arg)
{
	struct sockaddr_in serv_addr, cli_addr;
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(SENDPORT);

	cli_addr.sin_family = PF_INET;
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    cli_addr.sin_port = htons(33001);

	socklen_t clilen;
	clilen = sizeof(cli_addr);
	int len = 30;

    int Client_Socket, Bind_Socket, bytes_recv1, bytes_recv2;
    char buff2[MAXMSGLEN];
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

    printf("%i\n", *buff1);
    printf("%c\n", (char)buff2[0]);
    int Newqueue = (int) arg;
	if(msgrcv(Newqueue, &buff2, sizeof(struct msgbuf)-sizeof(long), 0,IPC_NOWAIT) == -1)
		DieWithError("Cant recieve from queue");

	send(Bind_Socket, buff2, len, 0);
	pthread_exit(0);

}*/