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
#include <sys/sem.h>

#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define RCVBUFSIZE 32   /* Size of receive buffer */
#define RECEIVEPORT 32000
#define SENDPORT 33000
#define MAXMSGLEN 30
#define MSGMAX 10
#define UDPPORT 16000

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

int count = 10;

struct message
{
    int TimeToSleep;
    int Number;
    char message[MAXMSGLEN];
};
	union semun {
		int val;   
		struct semid_ds *buf; 
		unsigned short *array; 
		struct seminfo *__buf; 
	};


void *UDP_Broadcast1(void* arg)
{
	int UDP_Socket;
	int msg;
	printf("%i\n", msg);
	struct sockaddr_in serv_addr, cli_addr;
	cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    cli_addr.sin_port = htons(28888);

    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(UDPPORT);

	if ((UDP_Socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    DieWithError("Cant socketing udp connection");
	int rc = bind(UDP_Socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (count > 0)
    {
    	msg = 1;
    	printf("%i\n", msg);
		sendto(UDP_Socket, &msg, sizeof(int), 0, (struct sockaddr*) &cli_addr, sizeof(cli_addr) );
	} else {
		msg = 0;
		sendto(UDP_Socket, &msg, sizeof(msg), 0, (struct sockaddr*) &cli_addr, sizeof(cli_addr) );
	}
}

//int UDP_Broadcast1(void* arg)




int main(int argc, char ** argv)
{	
	pthread_t TCP1, TCP2, UDP1, UDP2;
	key_t key = 10;
	int Newqueue = msgget(key, 0666 | IPC_CREAT);
	if (Newqueue < 0)
		DieWithError("Cant create queue");


	int result = pthread_create(&TCP1, NULL, ReceiveTCP, (void*)Newqueue);
		if (result != 0) {
		perror("Creating the first thread");
		return EXIT_FAILURE;
	}



	pthread_create(&TCP2, NULL, SendTCP, (void*)Newqueue);

	pthread_create(&UDP1, NULL, UDP_Broadcast1, NULL);


	pthread_join(TCP1, NULL);


	pthread_join(TCP2, NULL);


	msgctl(Newqueue, IPC_RMID, NULL);

	return 0;
}

void *ReceiveTCP(void *arg)
{
	struct sockaddr_in serv_addr, cli_addr;
	int Client_Socket, Bind_Socket, bytes_recv1, bytes_recv2;

    int Newq = (int) arg;
    int Num;
    //printf("%i\n",Newq);
    //printf("bruh1");
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(RECEIVEPORT);

	cli_addr.sin_family = PF_INET;
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    cli_addr.sin_port = INADDR_ANY;

	socklen_t clilen;
	clilen = sizeof(cli_addr);



    Client_Socket = socket(PF_INET, SOCK_STREAM, 0);

    if(Client_Socket < 0)
    {
    DieWithError("Cant socket connection");
    }

	int ts = bind(Client_Socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if(ts < 0)
    {
    DieWithError("Cant bind connection");
    }


    listen(Client_Socket, 5);

    while(1){
	Bind_Socket = accept(Client_Socket, (struct sockaddr *) &cli_addr, &clilen);
    if(Bind_Socket < 0)
    {
        DieWithError("Cant accept connection");
    }
    printf("Accepted with ", inet_ntoa(cli_addr.sin_addr),"\n");
    bytes_recv1 = recv(Bind_Socket, &Num , sizeof(int), 0);

    printf("%i\n", Num);
    struct message
{
    int TimeToSleep;
    int Number;
    char message[Num];
};
	struct message buff;

    bytes_recv2 = recv(Bind_Socket, &buff , sizeof(buff), 0);
    count = count -1;
    printf("%i\n", count);


    printf("%i\n", buff.TimeToSleep);
    printf("%i\n", buff.Number);

//    for(int i = 0; i < buff.Number; i++)
//    	printf("%c", buff.message[i]);

    int test = msgsnd(Newq, &buff, sizeof(buff), 0);
	if(test == -1)
		DieWithError("Cant send to queue");
}
close(Client_Socket);
close(Bind_Socket);
pthread_exit(0);
}

void *SendTCP(void *arg)
{
	struct sockaddr_in serv_addr, cli_addr;
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(SENDPORT);

	cli_addr.sin_family = PF_INET;
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    cli_addr.sin_port = INADDR_ANY;;

	socklen_t clilen;
	clilen = sizeof(cli_addr);


    int Client_Socket, Bind_Socket, bytes_recv1, bytes_recv2;

    struct message buff1;
    if ((Client_Socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
    DieWithError("Cant accept connection");
    }
    if(bind(Client_Socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
    DieWithError("Cant accept connection");
    }
    listen(Client_Socket, 5);

//    while(1){
    if((Bind_Socket = accept(Client_Socket, (struct sockaddr *) &cli_addr, &clilen)) < 0)
    {
        DieWithError("Cant accept connection");
    }

    int Newq = (int) arg;
	if(msgrcv(Newq, &buff1, sizeof(buff1), 0,0) == -1)
		DieWithError("Cant recieve from queue");
//	printf("%i\n", buff1.Number);
//    printf("%i\n", buff1.TimeToSleep);
//        for(int i = 0; i < buff1.Number; i++)
//    	printf("%c", buff1.message[i]);
    	if((send(Bind_Socket, &buff1.Number, sizeof(int), 0)) < 0)
		DieWithError("Cant send it");

	if((send(Bind_Socket, &buff1, sizeof(buff1) - sizeof(long), 0)) < 0)
	{
		DieWithError("Cant send it");
	}else{
	count++;
	printf("%i\n", count);
//}
}
close(Client_Socket);
close(Bind_Socket);
pthread_exit(0);

}
