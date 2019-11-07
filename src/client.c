
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>

#include "common.h"
#include "msg_struct.h"

#include "liste_chaine.c"
#include "salon.c"


struct message edit_struct(struct message msg, int len, enum msg_type type, char *sender, char *infos)
{

    msg.pld_len = len;
    msg.type = type;
    strncpy(msg.nick_sender, sender, strlen(sender));
    strncpy(msg.infos, infos, strlen(infos));

    return msg;
}

int send_buff(int sockfd, char *buff)
{
    // Sending buffer
    if (send(sockfd, buff, strlen(buff), 0) <= 0)
    {
        return 0;
    }
    else
    {

        return 1;
    }
}

int send_struct(int sockfd, struct message msgstruct)
{

    /* Sending structure */
    if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void input(char *buff)
{
    fflush(stdout);
    fflush(stderr);

    int n = 0;
    while ((buff[n++] = getchar()) != '\n')
    {
    } // trailing '\n' will be sent
}

void echo_client(int sockfd)
{
    struct message msgstruct;
    struct pollfd fds[2];


    char *buff = malloc(MSG_LEN);
    char *rep_buff = malloc(MSG_LEN);
    char *nickname = malloc(NICK_LEN);
    nickname = "";
    char *old_nickname = malloc(NICK_LEN);
    char *mot = malloc(MSG_LEN);
    char *file = malloc(MSG_LEN);
    char *file_nickname = malloc(NICK_LEN);
    int file_check = 0;

    char salon[MSG_LEN];
    strcpy(salon, "");
    int i;

    char * acceptedChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";




    fds[0].fd = sockfd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    fds[1].fd = 0;
    fds[1].events = POLLIN;
    fds[1].revents = 0;

    while (1)
    {

        memset(&msgstruct, 0, sizeof(struct message));
        memset(buff, 0, MSG_LEN);
        memset(rep_buff, 0, MSG_LEN);

        if (poll(fds, 2, -1) < 1)
        {
            perror("Connection error: \n");
            exit(EXIT_FAILURE);
        }
        for (i = 0; i < 2; i++)
        {
            if (fds[i].revents == POLLIN)
            {

                if (fds[i].fd == sockfd) /* socket */
                {
                    /* Receiving msgstruct */
                    if (recv(fds[i].fd, &msgstruct, sizeof(struct message), 0) <= 0)
                    {
                        printf("msgstruct failure\n");
                        exit(EXIT_FAILURE);
                        break;
                    }

                    /* Receiving message */
                    if(msgstruct.pld_len > 0){
                      if (recv(fds[i].fd, buff, msgstruct.pld_len, 0) <= 0)
                      {
                          exit(EXIT_FAILURE);
                          break;
                      }

                      printf(">> %s", buff);
                      fflush(stdout);
                    }


                    switch (msgstruct.type)
                    {
                    case MULTICAST_CREATE:
                        strcpy(salon,msgstruct.infos);
                        break;
                    case MULTICAST_JOIN:
                        strcpy(salon,msgstruct.infos);
                        break;

                     case MULTICAST_QUIT:
                        strcpy(salon,"");
                        break;
                     case NICKNAME_NEW:
                        strcpy(nickname,msgstruct.infos);
                        break;                   

                    case FILE_REQUEST:
                      file_check = 1;
                      file_nickname = msgstruct.nick_sender;

                    default:
                        break;
                    }






                }
                else /* keyboard */
                {


                    input(buff);

                    if (!strncmp(buff, "/nick", 5))
                    {


                        int correct = 1;
                        int i;
                        for (i = 6; i < strlen(buff); i++)
                        {
                            if (buff[i] == ' ')
                            {
                                printf(">> Pseudo incorrect. \n");
                                correct = 0;
                                break;

                            }

                        }

                       if(correct){

                        if(strlen(nickname) > 127){
                          strcpy(rep_buff," ");
                          msgstruct = edit_struct(msgstruct, 1, NICKNAME_NEW, old_nickname, nickname);
                          nickname = (char *)malloc(strlen(old_nickname)+1);
                          strcpy(nickname, old_nickname);
                        }
                        else if(strspn(nickname, acceptedChars) != strlen(nickname)-1){
                          strcpy(rep_buff," ");
                          msgstruct = edit_struct(msgstruct, 1, NICKNAME_NEW, old_nickname, nickname);
                          nickname = (char *)malloc(strlen(old_nickname)+1);
                          strcpy(nickname, old_nickname);
                        }

                        memset(old_nickname, 0, NICK_LEN);
                        strcpy(old_nickname, nickname);

                        mot = strtok(buff, " ");
                        while (mot != NULL)
                        {
                            if (strcmp(mot, "/nick") == 0)
                            {
                                mot = strtok(NULL, " ");
                            }
                            else
                            {


                                nickname = (char *)malloc(strlen(mot) + 1);


                                strcpy(nickname, mot);

                                mot = strtok(NULL, "");
                            }

                        }
                              strcpy(rep_buff, " ");
                              msgstruct = edit_struct(msgstruct, 1, NICKNAME_NEW, old_nickname, nickname);
                        }

                    }





                    else if ( !strncmp(buff, "/whois",6))

                    {
                        mot = strtok(buff, " ");
                        while (mot != NULL)
                        {
                            if (strcmp(mot, "/whois"))
                            {
                                break;
                            }
                            mot = strtok(NULL, " ");
                        }
                        strcpy(rep_buff, " ");
                        msgstruct = edit_struct(msgstruct, 0, NICKNAME_INFOS, nickname, mot);
                    }



                    else if (!strncmp(buff, "/who", 4))
                    {
                        strcpy(rep_buff, " ");
                        msgstruct = edit_struct(msgstruct, 1, NICKNAME_LIST, nickname, " ");

                    }

                    else if (!strncmp(buff, "/msgall", 7))
                    {
                        mot = strtok(buff, " ");
                        while (mot != NULL)
                        {
                            if (strcmp(mot, "/msgall"))
                            {
                                break;
                            }
                            mot = strtok(NULL, " ");
                        }

                        strcpy(rep_buff, mot);
                        msgstruct = edit_struct(msgstruct, strlen(rep_buff), BROADCAST_SEND, nickname, "");
                    }

                    else if (!strncmp(buff, "/msg", 4))
                    {

                        int i = 0;
                        char pseudo[MSG_LEN];
                        char *mot = strtok(buff, " ");
                        while (mot != NULL)
                        {

                            if (!strncmp(mot, "/msg", 4))
                            {
                                mot = strtok(NULL, " ");
                            }
                            else
                            {
                                if (i == 0)
                                    strcpy(pseudo, mot);
                                if (i == 1)
                                    strcpy(rep_buff, mot);

                                mot = strtok(NULL, " ");

                                i++;
                            }
                        }
                        strcat(pseudo,"\n");
                        msgstruct = edit_struct(msgstruct, strlen(rep_buff), UNICAST_SEND, nickname, pseudo);
                    }

                    else if (!strncmp(buff, "/create", 7))
                    {

                        int correct = 1;
                        int i;
                        for (i = 8; i < strlen(buff); i++)
                        {
                            if (buff[i] == ' ')
                            {
                                printf(">> Nom de salon invalide. \n");
                                correct = 0;
                                break;

                            }

                        }

                       if(correct){

                        char *mot;
                        mot = strtok(buff, " ");
                        while (mot != NULL)
                        {
                            if (strcmp(mot, "/create"))
                            {
                                break;
                            }
                            mot = strtok(NULL, " ");
                        }


                        strcpy(rep_buff, "");
                        msgstruct = edit_struct(msgstruct, 0, MULTICAST_CREATE, nickname, mot);
                       }else{

                        msgstruct.type = ECHO_SEND ;
                       }
                    }

                    else if (!strncmp(buff, "/channel_list", 13))
                    {
                        strcpy(rep_buff, " ");
                        msgstruct = edit_struct(msgstruct, 1, MULTICAST_LIST, nickname, "");
                    }
                    else if (!strncmp(buff, "/join", 5))
                    {
                        char *mot;
                        mot = strtok(buff, " ");
                        while (mot != NULL)
                        {
                            if (strcmp(mot, "/join"))
                            {
                                break;
                            }
                            mot = strtok(NULL, " ");
                        }

                        strcpy(rep_buff, salon);

                        msgstruct = edit_struct(msgstruct, strlen(rep_buff), MULTICAST_JOIN, nickname, mot);
                    }

                    //Jalon 4
                    else if (!strncmp(buff, "/send", 5))
                    {

                        int i = 0;
                        char pseudo[MSG_LEN];
                        char *mot = strtok(buff, " ");
                        while (mot != NULL)
                        {

                            if (!strncmp(mot, "/send", 5))
                            {
                                mot = strtok(NULL, " ");
                            }
                            else
                            {
                                if (i == 0)
                                    strcpy(pseudo, mot);
                                if (i == 1)
                                    strcpy(rep_buff, mot);
                                    strcpy(file, mot);

                                mot = strtok(NULL, " ");

                                i++;
                            }
                        }
                        strcat(pseudo,"\n");
                        msgstruct = edit_struct(msgstruct, strlen(rep_buff), FILE_REQUEST, nickname, pseudo);
                    }

                    else if (!strncmp(buff, "N", 1) && file_check == 1){
                      file_check = 0;
                      strcat(file_nickname,"\n");
                      msgstruct = edit_struct(msgstruct, strlen(rep_buff), FILE_REJECT, nickname, file_nickname);
                      memset(file_nickname, 0, NICK_LEN);
                    }

                    else if (!strncmp(buff, "Y", 1) && file_check == 1){
                      file_check = 0;
                      strcat(file_nickname,"\n");
                      msgstruct = edit_struct(msgstruct, strlen(rep_buff), FILE_ACCEPT, nickname, file_nickname);
                      memset(file_nickname, 0, NICK_LEN);
                    }

                    else if (strncmp(buff, "/quit",5) == 0)
                    {

                        if (strcmp(salon, "") == 0)
                        {
                            strncpy(msgstruct.nick_sender, nickname, strlen(nickname));
                            msgstruct.type = ECHO_SEND;
                            strncpy(msgstruct.infos, "close/0", 8);
                            msgstruct.pld_len = strlen(msgstruct.infos);
                            close(sockfd);
                            printf("connexion closed! \n");
                            exit(EXIT_SUCCESS);
                        }
                        else
                        {

                            printf("Vous allez sortir du salon %s ", salon);

                            strcpy(rep_buff,"");

                            msgstruct = edit_struct(msgstruct, 0, MULTICAST_QUIT, nickname, salon);

                            strcpy(salon,"");




                        }



                    }

                    else
                    {

                        if (strcmp(salon, "") == 0)
                        {
                           /* Pas de /commande == ECHO_SEND */
                           strcpy(rep_buff, buff);
                           msgstruct = edit_struct(msgstruct, strlen(buff), ECHO_SEND, nickname, "");
                        }
                        else
                        {
                            strncpy(rep_buff,nickname,strlen(nickname) -1 );
                            //strcpy(rep_buff, nickname);
                            strcat(rep_buff, " : ");
                            strcat(rep_buff, buff);

                            //printf("Multicast dans %s \n", salon);
                            msgstruct = edit_struct(msgstruct, strlen(rep_buff), MULTICAST_SEND, nickname, salon);
                        }


                    }

                    if (strcmp(nickname, "") == 0)
                    {

                        printf(">> Please login with /nick <pseudo>\n");
                        continue;
                    }
                    else
                    {

                        if (strcmp(salon, "") == 0)
                        {

                           // printf("CLIENT : Youre not in any salon\n");
                        }
                        else
                        {

                            //printf("Vous etes dans %s", salon);
                        }

                        if (send_struct(sockfd, msgstruct))
                           // printf("STRUCT SENT:LEN:%i,TYPE:%s,SENDER:%s,INFOS:%s\n", msgstruct.pld_len, msg_type_str[msgstruct.type], msgstruct.nick_sender, msgstruct.infos);
                        send_buff(sockfd, rep_buff);
                           //   printf("BUFF SENT :%s\n", rep_buff);


                    }
                }
            }
        }
    }
}

int handle_connect(char *serveur_ip_adresse, char *serveur_port)
{
    struct addrinfo hints, *result, *rp;
    int sfd;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(serveur_ip_adresse, serveur_port, &hints, &result) != 0)
    {
        perror("getaddrinfo()");
        exit(EXIT_FAILURE);
    }
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
        {
            continue;
        }
        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
        {
            break;
        }
        close(sfd);
    }
    if (rp == NULL)
    {
        fprintf(stderr, "Could not connect\n");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result);
    return sfd;
}

int main(int argc, char *argv[])
{
    char *serveur_ip_adresse = malloc(sizeof(char));
    char *serveur_port = malloc(sizeof(char));

    if (argc == 3)
    {
        serveur_ip_adresse = argv[1];
        serveur_port = argv[2];
    }
    else
    {
        serveur_ip_adresse = "127.0.0.1";
        serveur_port = "8080";
    }

    int sfd;

    sfd = handle_connect(serveur_ip_adresse, serveur_port);

    printf("Connected to the server : %s:%s \n", serveur_ip_adresse, serveur_port);

    printf("Please login with /nick <pseudo>\n");

    echo_client(sfd);

    close(sfd);

    return EXIT_SUCCESS;
}
