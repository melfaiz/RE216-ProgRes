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
    char hello[100] = "Hello from the client";
    struct sockaddr_in servaddr, cliaddr;

    /* Create socket file descriptor */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); //AF=INET : IPv4, SOUCK_DGRAM : UDP
    if (sockfd == -1) perror("socket creation :\n");

    /* Filling server info */
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // on accepte n'importe quelle adrss car c'est uns serveur
    servaddr.sin_family = AF_INET; //AF=INET : IPv4
    servaddr.sin_port = htons(33000);

    /* Bind the socket with the server address */

    bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)); // SOCKADDR) == (struct sockaddr *)

    /* Loop to handle data from/to a client */
    while (1){
      // pour UDP on n'utilise pas send et reveive mais sendto et receivefrom

        /* message to receive from the client */
        char message[MSG_SIZE];
        memset(message, 0, MSG_SIZE);

        // Recv data from the client
        int len = 0;
        int n = recvfrom(sockfd, (char *)message, MSG_SIZE, MSG_WAITALL, (struct sockaddr *) &cliaddr, (socklen_t *)&len);
        message[n] = '\0';
        printf("Client : %s\n", message);

        // Send back the data to the client
          /* Create message for the server */
          char reply[MSG_SIZE];
          memset(reply, 0, MSG_SIZE);
          strcpy(reply, "Hello from the server\n");

        int ret = 0;
        ret = sendto(sockfd, reply, strlen(reply), 0, (struct sockaddr *) &servaddr, sizeof(struct sockaddr));
        if(ret == -1)
          perror("sendto error:");
        else
          printf("Bytes sent : %d and reply is: %s\n", ret, reply);



    }

    return 0;
}
