
#include <stdio.h>
#include <stdlib.h>



struct Client {
	char* pseudo;
	int port;
	char* addresse;
	int descripteur;
  struct Client *suivant;
};

struct Liste{
   struct Client *premier;
};

struct Client* liste_get_premier(struct Liste* liste) {
	return liste->premier;
}

char* client_get_pseudo(struct Client* client) {
	return client->pseudo;
}

int client_get_port(struct Client* client) {
	return client->port;
}

int client_get_descripteur(struct Client* client) {
	return client->descripteur;
}

char* client_get_addresse(struct Client* client) {
	return client->addresse;
}

struct Client* client_get_next(struct Client* client){
	return client->suivant;
}

struct Liste* initialisation(int port, int descripteur, char* addresse){
  struct Liste *liste = malloc(sizeof(*liste));
  struct Client *client = malloc(sizeof(*client));
  if (liste == NULL || client == NULL){
    exit(EXIT_FAILURE);
  }
  client->port = port;
	client->addresse = malloc(20*sizeof(char));
  sprintf(client->addresse, addresse);
	client->pseudo = malloc(128*sizeof(char));
	sprintf(client->pseudo, "Unknow");
  client->descripteur = descripteur;
  client->suivant = NULL;
  liste->premier = client;
  return liste;
}

void ajouter(int port, int descripteur, char* addresse, struct Liste *liste){
    /* Création du nouvel élément */
    struct Client *nouveau = malloc(sizeof(*nouveau));
    nouveau->port = port;
    nouveau->descripteur = descripteur;
		nouveau->addresse = malloc(20*sizeof(char));
	  sprintf(nouveau->addresse, addresse);
		nouveau->pseudo = malloc(128*sizeof(char));
		sprintf(nouveau->pseudo, "Unknow");
    /* Insertion de l'élément au début de la liste */
    nouveau->suivant = liste->premier;
    liste->premier = nouveau;
}

void ajouter_pseudo(int descripteur, char* pseudo, struct Liste *liste){
	struct Client* element = liste->premier;
	while(client_get_descripteur(element) != descripteur){
		element = client_get_next(element);
	}
		sprintf(element->pseudo, pseudo);
}

void supprimer(int descripteur, struct Liste *liste){
	struct Client* element_supprime = liste->premier;
	if(client_get_descripteur(element_supprime) == descripteur){
		printf("ok\n");
		liste->premier = element_supprime->suivant;
		if(element_supprime != NULL){
			free(element_supprime);
		}
	}
	else{
		printf("OK\n");
		struct Client *element_supprime_precedent = element_supprime;
		element_supprime = element_supprime->suivant;
		struct Client *element_supprime_suivant = element_supprime->suivant;
		while(client_get_descripteur(element_supprime) != descripteur){
			element_supprime_precedent = element_supprime;
			element_supprime = client_get_next(element_supprime);
			element_supprime_suivant = element_supprime->suivant;
		}
		if(element_supprime != NULL){
			element_supprime_precedent->suivant = element_supprime_suivant;
			free(element_supprime);
		}
	}
}

int check_pseudo(int descripteur, char* pseudo, struct Liste *liste){
	int check = 0;
	struct Client* element_check = liste->premier;
	if(strcmp(client_get_pseudo(element_check), pseudo) == 0 && client_get_descripteur(element_check) != descripteur){
		check = 1;
	}
	while(client_get_next(element_check) != NULL){
		element_check = client_get_next(element_check);
		if(strcmp(client_get_pseudo(element_check), pseudo) == 0 && client_get_descripteur(element_check) != descripteur){
			check = 1;
		}
	}
	return check;
}

int descripteur(char* pseudo, struct Liste *liste){
	struct Client* element = liste->premier;
	if(strcmp(client_get_pseudo(element), pseudo) == 0){
		return client_get_descripteur(element);
	}
	while(client_get_next(element) != NULL){
		element = client_get_next(element);
		if(strcmp(client_get_pseudo(element), pseudo) == 0){
			return client_get_descripteur(element);
		}
	}
	return -1;
}

