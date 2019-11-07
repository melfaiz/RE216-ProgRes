#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"



typedef struct Salon {
	char* nom;
    int fds[100];
    int nb_fds;
    struct Salon* suivant;
}Salon;


int salon_existe(Salon* head, char* nom){

    if(head->suivant == NULL){
        printf("PAS DE SALON\n");
        return 0;
    }
    else{
        
        while( head->suivant != NULL){
            if( strcmp(head->nom,nom) == 0 && head->nb_fds != -1 ){
                return 1;
            }
            head = head->suivant;
        }
    

        return 0;
    }
}


Salon* nouveau_salon(Salon* head, char* nom){


    Salon* nv_salon = malloc(sizeof(*nv_salon));

    nv_salon->nom = malloc(strlen(nom)+1); // taille du nom de salon
    strcpy(nv_salon->nom,nom);

    nv_salon->nb_fds = 0;

    nv_salon->suivant = head ;

    return nv_salon;
}



char* print_salons(Salon* head){

    char* salons = malloc(MSG_LEN);

    if(head->suivant == NULL){
        
        printf("PAS DE SALON\n");
        strcpy(salons,"");
    }
    else{
        strcpy(salons,"Les salons sont : \n");
        
        while( head->suivant != NULL){

            int nb = head->nb_fds ;         
            
            

            if( nb != -1){

                strcat(salons,"- ");
                strcat(salons,head->nom);
                
            }
            head = head->suivant;
        }
    }

    if(strlen(salons) < 22)
        strcpy(salons,"Pas encore de salons. \n"); 


    return salons;
}


void join_salon(Salon* head, char* nom,int fd){

    if(head == NULL){
        printf("Pas encore de salon\n");
    }
    else{
        while( head->suivant != NULL){

            if( !strcmp(head->nom, nom) && head->nb_fds != -1){

                
                head->fds[ head->nb_fds ] = fd;     
                head->nb_fds =  head->nb_fds + 1 ;   

                break;

            }
            head = head->suivant;


        }
    }

}

void quit_user(Salon* head,char* salon,int fd){

    if(head == NULL){
        printf("Pas encore de salon\n");
    }
    else{
        while( head->suivant != NULL){

            if( !strcmp(head->nom, salon)){

                

                int* fds = head->fds;

                int i;

                printf("nbfd : %i \n",head->nb_fds);

                for ( i = 0; i < head->nb_fds; i++)
                {
                    
                    if (fds[i] == fd)
                    {
                        
                        int c;
                        for (c = i; c < head->nb_fds - 1 ; c++)
                            fds[c] = fds[c+1];

                        head->nb_fds = head->nb_fds -1 ;

                        if(head->nb_fds == 0){

                            head->nb_fds = -1;
                            
                            printf("DELETED \n");
                        }
                            

                        printf("DECONECTED \n");

                    }
   
                }
                

                break;

            }
            head = head->suivant;


        }
    }   
}






Salon* salon_by_name(Salon* head, char* nom){

     if(head->suivant == NULL){
        printf("PAS DE SALON\n");
        
    }
    else{
        
        while( head->suivant != NULL){
            if( strcmp(head->nom,nom) == 0 ){
                
                return head;
            
            }
            head = head->suivant;
        }
    

       
    }
}

Salon* salons ;
