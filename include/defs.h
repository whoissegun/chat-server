#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>

#define MAX_LINE 4096
#define MAX_NAME 10
#define MAX_CLIENTS 10

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define SERVER_PORT 3000

typedef struct{
    int sockfd;
    struct sockaddr_in clientAddr;
    char username[MAX_NAME];
    pthread_t recvThreadId; //thread id for receiving messages
    pthread_t sendThreadId; //thread id for sending messages
} ClientType;

typedef struct {
    int sockfd;
    struct sockaddr_in serverAddr;
    pthread_t handleClientIds[MAX_CLIENTS]; //thread ids for handling clients
} ServerType;

extern ClientType *clients; //array of clients
extern int numClients; //number of clients

void print_error(char *msg);  // Print error message and exit

//client functions
void runClient(char ipAddr[MAX_LINE]);
void initClient(ClientType *client, char ipAddr[MAX_LINE]);
void *receiveMessage(void* arg);
void *sendMessage(void* arg);
void *handleClient(void *arg);
void cleanupClient(ClientType *client);


//server functions
void runServer();
void initServer(ServerType *server);
void acceptClient(ServerType *server);
void broadcastMessage(char *msg);
void handleClientMessages(ServerType *server);
void cleanupServer(ServerType *server);

void sigintHandler(int dummy); // SIGINT handler
void clientsigintHandler(int dummy); // SIGINT handler for client