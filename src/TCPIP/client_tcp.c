#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>


int main(int argc, char *argv[]) {

  // créer une sequet avec socket:
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
      printf("le point de connexion n'est pas creé \n");
      return 1;
    }


     // get addrinfo:

    struct addrinfo hints;
    struct addrinfo *resultat;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = 0;
    int resultGetAddrInfo = getaddrinfo("127.0.0.1", "33000", &hints, &resultat );
    if (resultGetAddrInfo == -1 || resultat == NULL ){
      printf("erreur adresse \n");
      return 1;
    }


    // connect:

    int con = connect(sockfd, resultat->ai_addr, resultat->ai_addrlen);
    if (con == -1){
      perror("connect error");
    }



    // send:
    char *message = "salut tout le monde!";
    int resultatSend = send(sockfd , message , strlen(message),0);
    printf("Données envoyés :%d \n", resultatSend);



    //recv :
    int sizeMessage = 1024;
    char * received_msg = malloc(sizeMessage);
    int resultrecv = recv(sockfd, received_msg, sizeMessage, 0);
    if (resultrecv >0 )
    {
        printf("SERVEUR : %s\n", received_msg );
    }else
    {
        perror("Recieve error :");
    }
    
    
    

        
    free(received_msg);
    close(sockfd);

}