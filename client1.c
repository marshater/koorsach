#include <unistd.h>  /* TCP echo server includes */
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
#include <string.h>
#include <pthread.h>


#define MAX_LENGHT 30
#define MAX_SLEEPTIME 10
#define RECEIVEPORT 32000
#define RESPORT 10
#define TCPPORT 34000
#define UDPPORT 16000

pthread_mutex_t status = PTHREAD_MUTEX_INITIALIZER;

void dieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}


struct message
{
    int TimeToSleep;
    int Number;
    char message[MAX_LENGHT];
};

void *UDP_listener(void *arg);

void *SendTCP(void* arg)
{
    struct sockaddr_in client_addr = {0};
    struct sockaddr_in server_addr = {0};
    client_addr.sin_family = PF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(TCPPORT);

    server_addr.sin_family = PF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(RECEIVEPORT);
            
    struct message name; 

    srand(time(NULL));
    name.TimeToSleep = 1 + rand()%MAX_SLEEPTIME;
    name.Number = 1 + rand()%MAX_LENGHT;
    static char charset[] = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890";
    int lenght = name.Number;

    for(int i = 0; i < lenght; i++)
    {
        name.message[i] = charset[rand()%(strlen(charset))];
    }

    int Client_Socket;

    printf("%i\n", name.Number);
    printf("%i\n", name.TimeToSleep);
    for(int i = 0; i < name.Number; i++)
    {
        printf("%c", name.message[i]);
    }

	if ((Client_Socket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        dieWithError("Cant socket this");
    }

    while (connect(Client_Socket,(struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
    {
    	client_addr.sin_port = htons(TCPPORT+rand()%RESPORT);
    }

    send(Client_Socket, &name.Number, sizeof(name.Number), 0);
    send(Client_Socket, &name, sizeof(struct message), 0);
    sleep(name.TimeToSleep);
    pthread_exit(0);
}


int main(int argc, char ** argv)
{
	pthread_t UDP;

    int pc = pthread_create(&UDP, NULL, UDP_listener, NULL);
    if (pc != 0) 
    {
        dieWithError("Cant thread client1");
    }

    pthread_join(UDP, NULL);
    return 0;
}

void *UDP_listener(void *arg){
    pthread_t TCP1;
    int checker2;
    int lock = 0;
    struct sockaddr_in client_addr = {0};
    struct sockaddr_in server_addr = {0};
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = INADDR_ANY;
    client_addr.sin_port = htons(UDPPORT+1);

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(UDPPORT);
    int Client_Socket;


    int client_addr_size = sizeof(struct sockaddr_in);

    for(;;) 
    {
        if ((Client_Socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        {
            dieWithError("Cant bind connection");
        }

        while (bind(Client_Socket, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0)
        {
            client_addr.sin_port = htons(UDPPORT+rand()%RESPORT);
        }

    
        pthread_mutex_lock(&status);

        if((recvfrom(Client_Socket, &checker2, sizeof(checker2), 0, (struct sockaddr*) &server_addr, &client_addr_size)) > 0 ) 
        {
            pthread_create(&TCP1, NULL, SendTCP, NULL);
            pthread_join(TCP1, NULL);
            pthread_mutex_unlock(&status);
            close(Client_Socket);
        }
    }
}