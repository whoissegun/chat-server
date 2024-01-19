#include "../../include/defs.h"

volatile sig_atomic_t keepRunning = 1;

void sigintHandler(int dummy) {
    keepRunning = 0;
}



void runServer(){   
    signal(SIGINT, sigintHandler);
    ServerType *server = (ServerType *)malloc(sizeof(ServerType));
    if (!server)
    {
        print_error("Error allocating memory for server");
    }
    initServer(server);

    while (keepRunning){
        acceptClient(server);
    };

    cleanupServer(server);
}

void initServer(ServerType *server)
{
    server->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    numClients = 0;
    if (server->sockfd < 0)
    {
        print_error("Error opening socket");
    }

    // Initialize socket structure
    memset(&server->serverAddr, 0, sizeof(server->serverAddr));
    server->serverAddr.sin_family = AF_INET;
    server->serverAddr.sin_addr.s_addr = INADDR_ANY;
    server->serverAddr.sin_port = htons(SERVER_PORT);

    //make socket non-blocking
    int flags = fcntl(server->sockfd, F_GETFL, 0); //get current flags
    
    if(flags < 0){
        print_error("Error getting socket flags");
    }

    flags = flags | O_NONBLOCK; //add non-blocking flag to current flags

    if(fcntl(server->sockfd, F_SETFL, flags) < 0){
        print_error("Error setting socket flags");
    }

    if (bind(server->sockfd, (struct sockaddr *)&server->serverAddr, sizeof(server->serverAddr)) < 0){
        print_error("Error on binding");
    }

    for (int i = 0; i < MAX_CLIENTS; i++){
        clients[i].sockfd = -1;
    }

    if (listen(server->sockfd, MAX_CLIENTS) < 0)
    {
        print_error("Error on listen");
    }

    printf("Server started\n");
}

void acceptClient(ServerType *server)
{
    struct sockaddr_in clientAddr;
    int clientLen = sizeof(clientAddr);
    int newsockfd = accept(server->sockfd, (struct sockaddr *)&clientAddr, (socklen_t *)&clientLen);

    if (newsockfd < 0)
    {
        if(errno == EWOULDBLOCK || errno == EAGAIN){ // No client is trying to connect
            return;
        }

        print_error("Error on accept");
        return;
    }

    if (numClients >= MAX_CLIENTS)
    {
        print_error("Server full");
        close(newsockfd);
        return;
    }

    // Find an empty slot for the new client
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].sockfd == -1)
        {
            clients[i].sockfd = newsockfd;
            clients[i].clientAddr = clientAddr;
            numClients++;

            // Create and detach a new thread for handling the client
            if (pthread_create(&server->handleClientIds[i], NULL, (void *)handleClient, &clients[i]) != 0) {
                print_error("Error creating client thread");
                close(newsockfd);
                clients[i].sockfd = -1;
                numClients--;
                return;
            }
            pthread_detach(server->handleClientIds[i]);
            printf("Client connected\n");
            break;
        }
    }
}

void broadcastMessage(char *msg)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (clients[i].sockfd != -1)
        {
            int n = send(clients[i].sockfd, msg, strlen(msg), 0);
            if (n < 0)
            {
                close(clients[i].sockfd);
                print_error("Error writing to socket");
            }
        }
    }
}

// void handleClientMessages(ServerType *server) {
//     char msg[MAX_LINE];

//     for (int i = 0; i < MAX_CLIENTS; i++) {
//         if (clients[i].sockfd != -1) {
//             int n = recv(clients[i].sockfd, msg, MAX_LINE, 0);
//             if (n > 0) {
//                 printf("Received message from client %d: %s\n", i, msg);
//                 broadcastMessage(msg);
//             } else if (n < 0 && errno != EWOULDBLOCK) {
//                 close(clients[i].sockfd);
//                 clients[i].sockfd = -1;
//                 print_error("Error reading from socket");
//             }
//         }
//     }
// }

void cleanupServer(ServerType *server)
{
    // Close the server socket
    if (server->sockfd >= 0) {
        close(server->sockfd);
    }

    // Iterate through the clients and clean up
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].sockfd != -1) {
            // Close the client socket
            close(clients[i].sockfd);
            clients[i].sockfd = -1;
            
            // Cancel the client thread
            pthread_cancel(server->handleClientIds[i]);
            pthread_join(server->handleClientIds[i], NULL);

            numClients--;
        }
    }

    // Free the server structure if it was dynamically allocated
    free(clients);
    free(server);
}


void *handleClient(void *arg) {
    ClientType *client = (ClientType *)arg;

    // Main loop for handling client communication
    while (1) {
        char msg[MAX_LINE];

        // Receiving messages from client
        int n = recv(client->sockfd, msg, MAX_LINE, 0);
        if (n > 0) {

            if(msg[n]  != '\0'){ //if message is not null terminated, it means that the recv function has not copied the entire message
                continue;;
            }

            // broadcast to other clients
            broadcastMessage(msg);
        } else if (n == 0) {
            // Client has closed the connection
            printf("Client disconnected\n");
            cleanupClient(client);
            break;
        } else {
            // Error occurred
            if (errno != EWOULDBLOCK && errno != EAGAIN) {
                print_error("Error reading from socket");
                break;
            }
        }
    }

    
    numClients--;

    return NULL;
}
