#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>

#include <poll.h>

int main(int argc, char **argv)
{

    // Socket TCP:
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        printf("le point de connexion n'est pas creé \n");
        return 1;
    }

    // struct sockaddr_in;
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr)); // remplir toutes la structure par es 0
    saddr.sin_family = AF_INET;       // en IPV4
    saddr.sin_port = htons(33000);    // le port doit etre ecrit en binaire
    saddr.sin_addr.s_addr = INADDR_ANY;

    // bind() lui dire ecouter

    int resultBind = bind(sock, (struct sockaddr *)&saddr, sizeof(saddr)); // 2eme argument on caste sockaddr_in en sockaddr
    if (resultBind == -1)
    {
        perror("erreur au bind");
        return 1;
    }

    // listen:
    int backlog = 10;
    int list = listen(sock, backlog);

    if (list == -1)
    {
        perror("erreur au listen");
        return 1;
    }

    // structure de pollfds
    short nfds = 10;
    struct pollfd *fds = malloc(nfds * sizeof(struct pollfd));

    fds[0].fd = sock;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    int n;
    struct sockaddr addr;
    int len = sizeof(addr);

    while (1)
    {   
        n = poll(fds,1,0);


        for (size_t i = 0; i < nfds; i++)
        {
            if (fds[0].revents == POLLIN)
            {
                    //accept:

                    int sock_client = accept(sock, &addr, (socklen_t *)&len);
                    if (sock_client == -1)
                    {
                        perror("erreur au accept");
                        return 1;
                    }

                      
            }
            
        }
        
        
    }
    
    



    //recv:
    int sizeMessage = 1024;
    char *received_msg = malloc(sizeMessage);
    int resultrecv = recv(sock_client, received_msg, sizeMessage, 0);
    printf("CLIENT : %s\n", received_msg);

    // send back :

    char *message = "Message bien recu au serveur";
    int resultatSend = send(sock_client, message, strlen(message), 0);
    printf("Données envoyés :%d  \n", resultatSend);
    if (resultatSend == -1)
    {
        perror("send back error :");
    }
}