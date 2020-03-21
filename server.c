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
#include <semaphore.h>

#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define RCVBUFSIZE 32   /* Size of receive buffer */
#define RECEIVEPORT 32000
#define SENDPORT 33000
#define MAXMSGLEN 30
#define MSGMAX 10
#define UDPPORT 16000
#define UDPPORT2 17000
#define RESPORT 10

pthread_mutex_t tcprcv, tcpsnd = PTHREAD_MUTEX_INITIALIZER;

	struct sembuf lock_res = {0, -1, 0};
	struct sembuf rel_res = {0, 1, 0};


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
void *UDP_Broadcast1(void* arg);
void *UDP_Broadcast2(void* arg);
void *TCP_SENDER(void *arg);
void *TCP_RECIEVER(void *arg);

int count = 0;

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
struct info
{
    int Bind_Socket;
    int NumQueue;
};





int main(int argc, char ** argv)
{	
	pthread_t TCP1, TCP2, UDP1, UDP2;
	key_t key = 9;
	int Newqueue = msgget(key, 0666 | IPC_CREAT);
	if (Newqueue < 0)
		DieWithError("Cant create queue");

	union semun arg;
	key_t key2 = 20;
	int semid = semget(key2, 1, 0666 | IPC_CREAT);

	arg.val = 20;

	semctl(semid, 0, SETVAL, arg);

	int result = pthread_create(&TCP1, NULL, ReceiveTCP, (void*)Newqueue);
		if (result != 0) {
		perror("Creating the first thread");
		return EXIT_FAILURE;
	}



	pthread_create(&TCP2, NULL, SendTCP, (void*)Newqueue);

	pthread_create(&UDP1, NULL, UDP_Broadcast1, NULL);

	pthread_create(&UDP2, NULL, UDP_Broadcast2, NULL);

	pthread_join(TCP1, NULL);


	pthread_join(TCP2, NULL);
	pthread_join(UDP1, NULL);
	pthread_join(UDP2, NULL);


	msgctl(Newqueue, IPC_RMID, NULL);

	return 0;
}

void *ReceiveTCP(void* arg)
{
	struct sockaddr_in serv_addr, cli_addr;
	int Client_Socket, Bind_Socket, bytes_recv1, bytes_recv2;
	pthread_t TCP4;
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
    struct info TCPinfo;
    TCPinfo.NumQueue = (int) arg;
    TCPinfo.Bind_Socket = Bind_Socket;
    pthread_create(&TCP4, NULL, *TCP_RECIEVER, &TCPinfo);
    pthread_join(TCP4, NULL);


}
	close(Client_Socket);
	close(Bind_Socket);
	//pthread_exit(0);
}

void *SendTCP(void *arg)
{
	struct sockaddr_in serv_addr, cli_addr;
	serv_addr.sin_family = PF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(SENDPORT);
	pthread_t TCP3;
	cli_addr.sin_family = PF_INET;
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    cli_addr.sin_port = INADDR_ANY;;

	socklen_t clilen;
	clilen = sizeof(cli_addr);


    int Client_Socket, Bind_Socket, bytes_recv1, bytes_recv2;

    if ((Client_Socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
    DieWithError("Cant socket connection 2");
    }
    if(bind(Client_Socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
    DieWithError("Cant bind connection 2");
    }
    listen(Client_Socket, 5);

    while(1)
    {
    	Bind_Socket = accept(Client_Socket, (struct sockaddr *) &cli_addr, &clilen);


    	struct info TCPinfo;
    	TCPinfo.NumQueue = (int) arg;
    	TCPinfo.Bind_Socket = Bind_Socket;
    	pthread_create(&TCP3, NULL, *TCP_SENDER, &TCPinfo);
    	pthread_join(TCP3, NULL);
		close(Client_Socket);
		close(Bind_Socket);
	//pthread_exit(0);
		}
}

void *UDP_Broadcast1(void* arg)
{
	int UDP_Socket;
	int locker1 = 1;
	//printf("%i\n", msg);
	struct sockaddr_in serv_addr, cli_addr;
	cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = INADDR_ANY;


    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(UDPPORT);
	if ((UDP_Socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    DieWithError("Cant socketing udp connection");
	int rc = bind(UDP_Socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

	while(1){
	for (int i = 0; i < RESPORT; i++)
	{
		cli_addr.sin_port = htons(UDPPORT+i+1);
//		if (count < 10)
		sendto(UDP_Socket, &locker1, sizeof(int), 0, (struct sockaddr*) &cli_addr, sizeof(cli_addr) );
	} 
	//close(UDP_Socket);
	//pthread_exit(0);
}

}


void *UDP_Broadcast2(void* arg)
{
	int UDP_Socket;
	int locker2 = 1;
//	printf("%i\n", msg);
	struct sockaddr_in serv_addr, cli_addr;
	cli_addr.sin_family = AF_INET;
    cli_addr.sin_addr.s_addr = INADDR_ANY;
    

    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(UDPPORT2);

	if ((UDP_Socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
    DieWithError("Cant socketing udp connection 2");
	int rc = bind(UDP_Socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

	while(1){
	for (int i = 0; i < RESPORT; i++)
	{
	cli_addr.sin_port = htons(UDPPORT2+i+1);
//		if (count > 0)
		sendto(UDP_Socket, &locker2, sizeof(int), 0, (struct sockaddr*) &cli_addr, sizeof(cli_addr) );
	}
	}
	//close(UDP_Socket);
	//pthread_exit(0);
}
void *TCP_SENDER(void *arg)
{
	struct info* mes = (struct info*) arg;

    struct message buff2;

	int Newq = mes->NumQueue;
	int Bind_Socket = mes->Bind_Socket;

	pthread_mutex_lock(&tcpsnd);
	if(msgrcv(Newq, &buff2, sizeof(struct message), 0,0) == -1)
		DieWithError("Cant recieve from queue");
	printf("%i\n", buff2.Number);
    printf("%i\n", buff2.TimeToSleep);
//        for(int i = 0; i < buff1.Number; i++)
//    	printf("%c", buff1.message[i]);
//    send(Bind_Socket, &buff1.Number, sizeof(int), 0);


	send(Bind_Socket, &buff2, sizeof(buff2) - sizeof(long), 0);

			count--;
//			printf("%i\n", count);
			pthread_mutex_unlock(&tcpsnd);
			sleep(buff2.TimeToSleep);
			close(Bind_Socket);

}

void *TCP_RECIEVER(void *arg)
{
	struct info* mes = (struct info*) arg;



	int Newq = mes->NumQueue;
	int Bind_Socket = mes->Bind_Socket;
	int Num;
//	recv(Bind_Socket, &Num , sizeof(int), 0);
//	printf("%i\n", Num);

//    printf("%i\n", Num);
/*    struct message
{
    int TimeToSleep;
    int Number;
    char message[Num];
};*/
	struct message buff1;
	pthread_mutex_lock(&tcprcv);
	recv(Bind_Socket, &buff1 , sizeof(struct message), 0);

	count++;

    //printf("%i\n", count);


    //printf("%i\n", buff.TimeToSleep);
    //printf("%i\n", buff.Number);

//    for(int i = 0; i < buff.Number; i++)
//    	printf("%c", buff.message[i]);

    int test = msgsnd(Newq, &buff1, sizeof(struct message), 0);
	if(test == -1)
		DieWithError("Cant send to queue");
	pthread_mutex_unlock(&tcprcv);
	close(Bind_Socket);
}