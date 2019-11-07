
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "liste_chaine.c"
#include "salon.c"

#include "common.h"
#include "msg_struct.h"

#define CLIENT_NBR 256


void echo_server(int sfd, struct pollfd * fds, socklen_t lenaddr, struct sockaddr_in cli, int lenfd) {

	struct Liste* liste = NULL;
	char * acceptedChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	char * buff = malloc(MSG_LEN);
	char * buff_response = malloc(MSG_LEN);
	struct message msgstruct;

	salons = malloc(sizeof(*salons));
	salons->suivant = NULL;
	 // initialisation du HEAD des salons */

	while (1) {

		int pollvalue, sockfd;
		if ((pollvalue = poll(fds, CLIENT_NBR, -1)) == -1) {
			perror("poll()");
			break;
		};
		int i = 0;

		for ( i = 0; i < CLIENT_NBR; i++) {
			if (fds[i].revents == POLLIN) {
				if (fds[i].fd == sfd) {
					lenaddr = sizeof(cli);
					if ((sockfd = accept(sfd, (struct sockaddr*) &cli, &lenaddr)) < 0) {
						perror("accept()\n");
						exit(EXIT_FAILURE);
					}

					//liste chainée
					if(liste == NULL){
						liste = initialisation(cli.sin_port, sockfd, inet_ntoa(cli.sin_addr));
					}
					else{
						ajouter(cli.sin_port, sockfd, inet_ntoa(cli.sin_addr), liste);
					}
					fflush(stdout);
					fds[lenfd].fd = sockfd;
					fds[lenfd].events = POLLIN;
					lenfd += 1;
				}

				else{
					// Cleaning memory
					memset(buff, 0, MSG_LEN);
					memset(buff_response, 0, MSG_LEN);
					memset(&msgstruct, 0, sizeof(msgstruct));

					// Receiving message

					if (recv(fds[i].fd, &msgstruct, sizeof(struct message), 0) <= 0) {
						printf("[Server] : Connection closed (struct)\n");
						supprimer(fds[i].fd, liste);
						fflush(stdout);
						close(fds[i].fd);
						fds[i].fd = -1;
						break;
					}

			//		printf("STRUCT RCVT:LEN:%i,TYPE:%s,SENDER:%s,INFOS:%s\n", msgstruct.pld_len, msg_type_str[msgstruct.type], msgstruct.nick_sender, msgstruct.infos);

					if(msgstruct.pld_len > 0){
						if (recv(fds[i].fd, buff, msgstruct.pld_len, 0) <= 0) {
							printf("[Server] : Connection closed (buff)\n");
							fflush(stdout);
							close(fds[i].fd);
							fds[i].fd = -1;
							break;
						}
					}

					if(msgstruct.type == NICKNAME_NEW){

						if(check_pseudo(fds[i].fd, msgstruct.infos, liste) == 1){
							strcpy(buff_response, "Pseudo already exists\n");
							msgstruct.pld_len = strlen(buff_response);
							
							strcpy(msgstruct.infos, "");
							send(fds[i].fd, &msgstruct, sizeof(struct message), 0);
							send(fds[i].fd, buff_response, msgstruct.pld_len, 0);
						}
						else{
							if(strlen(msgstruct.infos) == INFOS_LEN){;
								strcpy(buff_response, "Pseudo too long\n");
								msgstruct.pld_len = strlen(buff_response);
								
								strcpy(msgstruct.infos, "");
								send(fds[i].fd, &msgstruct, sizeof(struct message), 0);
								send(fds[i].fd, buff_response, msgstruct.pld_len, 0);
							}
							else{
								if(strspn(msgstruct.infos, acceptedChars) != strlen(msgstruct.infos)-1){
									strcpy(buff_response, "Pseudo invalide\n");
									msgstruct.pld_len = strlen(buff_response);
									
									strcpy(msgstruct.infos, "");
									send(fds[i].fd, &msgstruct, sizeof(struct message), 0);
									send(fds[i].fd, buff_response, MSG_LEN, 0);
								}
								else{
									ajouter_pseudo(fds[i].fd, msgstruct.infos, liste);
									strcpy(buff_response, "Pseudo accepted\n");
									msgstruct.pld_len = strlen(buff_response);
									send(fds[i].fd, &msgstruct, sizeof(struct message), 0);
									send(fds[i].fd, buff_response, msgstruct.pld_len, 0);

								}
							}
						}
					}
					if(msgstruct.type == NICKNAME_LIST){
						struct Client * client = liste_get_premier(liste);
						strcat(buff_response, "Les utilisateurs connectés sont :\n");
						strcat(buff_response, "- ");
						strcat(buff_response, client_get_pseudo(client));

						while(client_get_next(client) != NULL){
							client = client_get_next(client);
							strcat(buff_response, "- ");
							strcat(buff_response, client_get_pseudo(client));

						}
							msgstruct.pld_len = strlen(buff_response);
							send(fds[i].fd, &msgstruct, sizeof(struct message), 0);
							send(fds[i].fd, buff_response, msgstruct.pld_len, 0);
					}
					if(msgstruct.type == NICKNAME_INFOS){

						struct Client * client = liste_get_premier(liste);
						if(strcmp(client_get_pseudo(client), msgstruct.infos) == 0){
							sprintf(buff_response, "%s connected with IP address %s and port number %d \n", client_get_pseudo(client), client_get_addresse(client), client_get_port(client));
							msgstruct.pld_len = strlen(buff_response);
							send(fds[i].fd, &msgstruct, sizeof(struct message), 0);
							send(fds[i].fd, buff_response, MSG_LEN, 0);
						}
						while(client_get_next(client) != NULL){
							client = client_get_next(client);
							if(strcmp(client_get_pseudo(client), msgstruct.infos) == 0){
								sprintf(buff_response, "%s connected with IP address %s and port number %d \n", client_get_pseudo(client), client_get_addresse(client), client_get_port(client));
								msgstruct.pld_len = strlen(buff_response);
								send(fds[i].fd, &msgstruct, sizeof(struct message), 0);
								send(fds[i].fd, buff_response, MSG_LEN, 0);
							}
						}
					}
					if(msgstruct.type == BROADCAST_SEND){
						strcpy(buff_response, buff);
						struct Client* client = liste_get_premier(liste);
						printf("%s\n",client_get_pseudo(client));
						if(strcmp(client_get_pseudo(client), msgstruct.nick_sender) != 0){
							send(client_get_descripteur(client), &msgstruct, sizeof(struct message), 0);
							send(client_get_descripteur(client), buff_response, MSG_LEN, 0);
						}
						while(client_get_next(client) != NULL){
							client = client_get_next(client);
							printf("%s\n",client_get_pseudo(client));
							if(strcmp(client_get_pseudo(client), msgstruct.nick_sender) != 0){
								send(client_get_descripteur(client), &msgstruct, sizeof(struct message), 0);
								send(client_get_descripteur(client), buff_response, MSG_LEN, 0);
							}
						}
					}


					if(msgstruct.type == UNICAST_SEND){
						int envoie = 0;
						struct Client * client = liste_get_premier(liste);
						printf("%s",client_get_pseudo(client));
						printf("%s\n",msgstruct.infos);
						if(strcmp(client_get_pseudo(client), msgstruct.infos) == 0){
							strcpy(buff_response, buff);
							msgstruct.pld_len = strlen(buff_response);
							send(client_get_descripteur(client), &msgstruct, sizeof(struct message), 0);
							send(client_get_descripteur(client), buff_response, MSG_LEN, 0);
							envoie = 1;
						}
						while(client_get_next(client) != NULL){
							client = client_get_next(client);
							printf("%s",client_get_pseudo(client));
							printf("%s\n",msgstruct.infos);
							if(strcmp(client_get_pseudo(client), msgstruct.infos) == 0){
								strcpy(buff_response, buff);
								msgstruct.pld_len = strlen(buff_response);
								send(client_get_descripteur(client), &msgstruct, sizeof(struct message), 0);
								send(client_get_descripteur(client), buff_response, MSG_LEN, 0);
								envoie = 1;
							}
						}
						if (envoie == 0){
							strcpy(buff_response, "[Server] : Pseudo does not exist\n");
							msgstruct.pld_len = strlen(buff_response);

							send(fds[i].fd, &msgstruct, sizeof(struct message), 0);
							send(fds[i].fd, buff_response, MSG_LEN, 0);
						}
					}

					if(msgstruct.type == ECHO_SEND){
						// CaMSG_LENs echo
						strcpy(buff_response, buff);
						if(strcmp(buff_response, "/quit") == 0){
							send(fds[i].fd, &msgstruct, sizeof(struct message), 0);
							send(fds[i].fd, buff, strlen(buff), 0);
							printf("[Server] : Connection closed\n");
							supprimer(fds[i].fd, liste);
							fflush(stdout);
							close(fds[i].fd);
							fds[i].fd = -1;

						}

						else{
							send(fds[i].fd, &msgstruct, sizeof(struct message), 0);
							send(fds[i].fd, buff, strlen(buff), 0) ;
						}
					}


					if(msgstruct.type == MULTICAST_CREATE){


							if(salon_existe(salons, msgstruct.infos)){
								printf("SALON EXISTE \n");

								msgstruct.type=ECHO_SEND;


								strcpy(buff_response, "SALON EXISTE \n");
								msgstruct.pld_len = strlen(buff_response);


								send(fds[i].fd, &msgstruct, sizeof(struct message), 0);
								send(fds[i].fd, buff_response, strlen(buff_response), 0) ;

							}else{

							salons = nouveau_salon(salons,msgstruct.infos);
							join_salon(salons, msgstruct.infos, fds[i].fd);
							printf("ROOM %s CREATED \n", msgstruct.infos);

							strcat(buff_response, "Bienvenue dans le salon :");
							strcat(buff_response, msgstruct.infos);
							strcat(buff_response, "\n");

							msgstruct.pld_len = strlen(buff_response) ;

							send(fds[i].fd, &msgstruct, sizeof(struct message), 0);
							send(fds[i].fd, buff_response, strlen(buff_response), 0) ;
							}


					}
					if(msgstruct.type == MULTICAST_LIST){


							strcpy(buff_response, print_salons(salons));

							msgstruct.pld_len = strlen(buff_response) ;

							send(fds[i].fd, &msgstruct, sizeof(struct message), 0);
							send(fds[i].fd, buff_response, strlen(buff_response), 0) ;



					}
					if(msgstruct.type == MULTICAST_JOIN){

							if(!salon_existe(salons, msgstruct.infos)){
								printf("SALON INTROUVABLE \n");

								msgstruct.type=ECHO_SEND;


								strcpy(buff_response, "SALON INTROUVABLE \n");
								msgstruct.pld_len = strlen(buff_response);


								send(fds[i].fd, &msgstruct, sizeof(struct message), 0);
								send(fds[i].fd, buff_response, strlen(buff_response), 0) ;

							}else{

								char* old_salon = malloc(sizeof(buff));
								strcpy(old_salon,buff);

								join_salon(salons, msgstruct.infos, fds[i].fd);

								quit_user(salons,old_salon, fds[i].fd);

								strcat(buff_response, "Bienvenue dans le salon :");
								strcat(buff_response, msgstruct.infos);

								msgstruct.pld_len = strlen(buff_response) ;

								send(fds[i].fd, &msgstruct, sizeof(struct message), 0);
								send(fds[i].fd, buff_response, strlen(buff_response), 0) ;

							}

					}
					if(msgstruct.type == MULTICAST_SEND){

						char* salon = malloc(strlen(msgstruct.infos) + 1);
						strcpy(salon,msgstruct.infos); // nom du salon



						strcpy(buff_response,buff); // msg a envoyer

						//printf("Message %s \n",buff);



						Salon* head = salon_by_name(salons,salon);

						//printf("Multicast dans %s \n",head->nom);

						int* fds = malloc(sizeof(int)*head->nb_fds);
						fds = head->fds;

						//printf("Nombre des fds  %i \n",head->nb_fds);

						int i;
						for(i=0; i<head->nb_fds;i++){

							send(fds[i], &msgstruct, sizeof(struct message), 0);
							send(fds[i], buff, strlen(buff), 0) ;
						}







					}

					if(msgstruct.type == MULTICAST_QUIT){

						char* salon = malloc(strlen(msgstruct.infos) + 1);
						strcpy(salon,msgstruct.infos); // nom du salon

						printf("Salon : %s \n",msgstruct.infos);

						strcpy(buff_response,buff); // msg a envoyer



						Salon* head = salon_by_name(salons,salon);

						printf("%i IS QUITING %s \n",fds[i].fd,salon);

						quit_user(salons,salon, fds[i].fd);




						strcpy(buff_response, "Vous avez quité le salon ");
						strcat(buff_response, msgstruct.infos);

						msgstruct.pld_len = strlen(buff_response) ;

						send(fds[i].fd, &msgstruct, sizeof(struct message), 0);
						send(fds[i].fd, buff_response, strlen(buff_response), 0) ;



					}
					if(msgstruct.type == FILE_REQUEST){
						int envoie = 0;
						struct Client * client = liste_get_premier(liste);
						if(strcmp(client_get_pseudo(client), msgstruct.infos) == 0){
							sprintf(buff_response, "%s wants you to accept the transfer of the file named %s. Do you accept? [Y/N]", msgstruct.nick_sender, buff);
							msgstruct.pld_len = strlen(buff_response);
							send(client_get_descripteur(client), &msgstruct, sizeof(struct message), 0);
							send(client_get_descripteur(client), buff_response, MSG_LEN, 0);
							envoie = 1;
						}
						while(client_get_next(client) != NULL){
							client = client_get_next(client);
							if(strcmp(client_get_pseudo(client), msgstruct.infos) == 0){
								sprintf(buff_response, "%s wants you to accept the transfer of the file named %s. Do you accept? [Y/N]", msgstruct.nick_sender, buff);
								msgstruct.pld_len = strlen(buff_response);
								send(client_get_descripteur(client), &msgstruct, sizeof(struct message), 0);
								send(client_get_descripteur(client), buff_response, MSG_LEN, 0);
								envoie = 1;
							}
						}
						if (envoie == 0){
							strcpy(buff_response, "[Server] : Pseudo does not exist\n");
							msgstruct.pld_len = strlen(buff_response);
							send(fds[i].fd, &msgstruct, sizeof(struct message), 0);
							send(fds[i].fd, buff_response, MSG_LEN, 0);
						}
					}
					if(msgstruct.type == FILE_ACCEPT){
						struct Client * client = liste_get_premier(liste);
						if(strcmp(client_get_pseudo(client), msgstruct.infos) == 0){
							sprintf(buff_response, "%s:%d", client_get_addresse(client), client_get_port(client));
							msgstruct.pld_len = strlen(buff_response);
							send(client_get_descripteur(client), &msgstruct, sizeof(struct message), 0);
							send(client_get_descripteur(client), buff_response, MSG_LEN, 0);
						}
						while(client_get_next(client) != NULL){
							client = client_get_next(client);
							if(strcmp(client_get_pseudo(client), msgstruct.infos) == 0){
								sprintf(buff_response, "%s:%d", client_get_addresse(client), client_get_port(client));
								msgstruct.pld_len = strlen(buff_response);
								send(client_get_descripteur(client), &msgstruct, sizeof(struct message), 0);
								send(client_get_descripteur(client), buff_response, MSG_LEN, 0);
							}
						}
					}
					if(msgstruct.type == FILE_REJECT){
						struct Client * client = liste_get_premier(liste);
						if(strcmp(client_get_pseudo(client), msgstruct.infos) == 0){
							sprintf(buff_response, "%s cancelled file transfer", msgstruct.nick_sender);
							msgstruct.pld_len = strlen(buff_response);
							send(client_get_descripteur(client), &msgstruct, sizeof(struct message), 0);
							send(client_get_descripteur(client), buff_response, MSG_LEN, 0);
						}
						while(client_get_next(client) != NULL){
							client = client_get_next(client);
							if(strcmp(client_get_pseudo(client), msgstruct.infos) == 0){
								sprintf(buff_response, "%s cancelled file transfer", msgstruct.nick_sender);
								msgstruct.pld_len = strlen(buff_response);
								send(client_get_descripteur(client), &msgstruct, sizeof(struct message), 0);
								send(client_get_descripteur(client), buff_response, MSG_LEN, 0);
							}
						}
					}
				}
			}
		}
	}
}

int handle_bind(char * Port) {
	struct addrinfo hints, *result, *rp;
	int sfd;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, Port, &hints, &result) != 0) {
		perror("getaddrinfo()");
		exit(EXIT_FAILURE);
	}
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,
		rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}
		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
			break;
		}
		close(sfd);
	}
	if (rp == NULL) {
		fprintf(stderr, "Could not bind\n");
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(result);
	return sfd;
}

int main(int argc, char * argv[]) {

	if(argc != 2){
		printf("USAGE : ./programe_name #PORT\n");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in cli;
	int sfd, lenfd;
	socklen_t lenaddr;
	struct pollfd fds[CLIENT_NBR];
	memset(&fds, 0, sizeof(fds));

	sfd = handle_bind(argv[1]);
	if ((listen(sfd, SOMAXCONN)) != 0) {
		perror("listen()\n");
		exit(EXIT_FAILURE);
	}

	fds[0].fd = sfd;
	fds[0].events = POLLIN;
	lenfd = 1;

	lenaddr = sizeof(cli);

	echo_server(sfd, fds, lenaddr, cli, lenfd );
	close(sfd);
	return EXIT_SUCCESS;
}
