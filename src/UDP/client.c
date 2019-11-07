#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define MSG_SIZE 65636

int main(int argc, char const * argv[])
{

    /* Check program parameters */
	if(argc != 3){
		printf("USAGE : ./programe_name @IP #PORT\n");
		exit(EXIT_FAILURE);
	}

    /* Create socket */
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1) perror("socket creation :\n");

 	/* get struct sockaddr of the server wih the getaddrinfo function */
	struct addrinfo hints, *res, *p;
	int status;
	memset(&hints, 0, sizeof hints);
 	//hints.ai_family = AF_INET; // AF_INET to force version
    //hints.ai_socktype = SOCK_DGRAM; // SOCK_DGRAM to force UDP 
	status = getaddrinfo(argv[1], argv[2], &hints, &res);
	if (status){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
		return 2;
	}

    /* Get the appropriate addrinfo with AF_INET and SOCK_DGRAM */
	while (res->ai_family != AF_INET || res->ai_socktype != SOCK_DGRAM) { 
		if (res->ai_next != NULL)
			res = res->ai_next;
		else {
			printf("No available addresses fitting AF_INET and SOCK_DGRAM\n");
			return 0;
		}
	}

	/* Get the sockaddr_in from the sockaddr returned by getaddrinfo */
	struct sockaddr_in * server_sockaddr_ipv4 = (struct sockaddr_in *)res->ai_addr;
	printf("AF_INET and SOCK_DGRAM have been found on address %s\n", inet_ntoa(server_sockaddr_ipv4->sin_addr));

    /* Create message for the server */
	char message[MSG_SIZE];
	memset(message, 0, MSG_SIZE);
	strcpy(message, "Hello world\n");

    /* Send message to the server */
	int ret = 0;
	ret = sendto(fd, message, strlen(message), 0, (struct sockaddr *)server_sockaddr_ipv4, sizeof(struct sockaddr));
	if(ret == -1) 
		perror("sendto error:");
	else 
		printf("Bytes sent : %d and message is: %s\n", ret, message);

	/* Receive message from the server */	
	memset(message, 0, MSG_SIZE);
	int len = 0;
	int n = recvfrom(fd, (char *)message, MSG_SIZE, 0, (struct sockaddr *) &server_sockaddr_ipv4, (socklen_t *)&len); 
	message[n] = '\0'; 
	printf("Server : %s\n", message); 

    /* Close and free memory */
	close(fd);
	freeaddrinfo(res);
	return 0;
}