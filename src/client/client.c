#include "../../include/defs.h"



void runClient(char ipAddr[MAX_LINE]){
    ClientType *client = malloc(sizeof(ClientType));
    initClient(client, ipAddr);

    pthread_create(&client->recvThreadId, NULL, receiveMessage, client);
    pthread_create(&client->sendThreadId, NULL, sendMessage, client);

    // Wait for the threads to complete
    pthread_join(client->recvThreadId, NULL);
    pthread_join(client->sendThreadId, NULL);
}

void initClient(ClientType *client, char ipAddr[MAX_LINE]){
    printf("Type your name: ");
    if (fgets(client->username, MAX_NAME, stdin) == NULL) {
        print_error("Error reading username");
        return;
    }
    client->username[strlen(client->username) - 1] = '\0'; // Remove newline

    client->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(client->sockfd < 0)
    {
        print_error("Error creating socket");
        return;
    }

    memset(&client->clientAddr, 0, sizeof(client->clientAddr));
    client->clientAddr.sin_family = AF_INET;
    client->clientAddr.sin_port = htons(SERVER_PORT);

    if(inet_pton(AF_INET, ipAddr, &client->clientAddr.sin_addr) <= 0)
    {
        print_error("Invalid address");
        close(client->sockfd);
        return;
    }

    if(connect(client->sockfd, (struct sockaddr *) &client->clientAddr, sizeof(client->clientAddr)) < 0)
    {
        print_error("Error connecting to server");
        close(client->sockfd);
        return;
    }

    printf("Connected to server\n");    
}


void *receiveMessage(void *arg) {
    ClientType *client = (ClientType *) arg; //type cast to clienttype
    char msg[MAX_LINE];

    while(1){
        
        int n = recv(client->sockfd, msg, MAX_LINE, 0); // Receive message from server
        if (n > 0) {
            msg[n] = '\0'; // Null-terminate the received string
            printf("%s\n", msg);

        } else if (n < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // No data available right now
                continue;
            } else {
                // An actual error occurred
                print_error("Error reading from socket");
                break;
            }
        }
    }

    pthread_cancel(client->recvThreadId); // Cancel the sending thread
    pthread_join(client->recvThreadId, NULL); //release thread resources
    return NULL;
    
}

void *sendMessage(void *arg){
    ClientType *client = (ClientType *) arg;
    char msg[MAX_LINE];
    
    while(1){
        //printf("Type a message: ");
        
        if(fgets(msg, MAX_LINE, stdin) == NULL) break; //fgets returns NULL if there is an error or EOF
        if(strcmp(msg, "exit\n") == 0){
            break; 
        }else{
            if(strlen(msg) == 0 || strlen(msg) > MAX_LINE || strcmp(msg, "\n") == 0){
                printf("Invalid message\n");
                continue;
            }

            int usernameLen = strlen(client->username);
            char newMsg[MAX_LINE + usernameLen + 2]; //+2 for the colon and space
            sprintf(newMsg, "%s: %s", client->username, msg); //prevent buffer overflow by copying only MAX_LINE characters
            newMsg[strlen(newMsg) - 1] = '\0';
            
            
            int n = send(client->sockfd, newMsg, strlen(newMsg), 0); //send message to server for broadcasting
            
            if(n < 0){
                
                print_error("Error writing to socket");
            }

            printf("Message sent\n");
        }
    }

    pthread_cancel(client->sendThreadId); // Cancel the receiving thread
    pthread_join(client->sendThreadId, NULL); //release thread resources
    return NULL;
    
}

void cleanupClient(ClientType *client){
    close(client->sockfd);
}

