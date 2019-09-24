// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define MSG_SIZE 1024 

int main() { 

    /* Create useful variables */
    int sockfd; 
    char buffer[MSG_SIZE]; 
    char hello[100] = "Hello from the server"; 
    struct sockaddr_in servaddr, cliaddr; 

    /* Create socket file descriptor */
    

    /* Filling server info */

    /* Bind the socket with the server address */

    
    /* Loop to handle data from/to a client */
    while (1){

        // Recv data from the client

        // Send back the data to the client
    }

    return 0; 
} 