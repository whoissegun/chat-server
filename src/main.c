#include "../include/defs.h"
int numClients = 0;
ClientType *clients;

int main(int argc, char **argv){
    
    clients = malloc(MAX_CLIENTS *sizeof(ClientType));
    
    if(argc!=2){ //is server
        runServer();
    }else if(argc==2){ //is client
        runClient(argv[1]);
    }else{
        print_error("Invalid arguments");
    }
    return EXIT_SUCCESS;
}