#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>

#define LIFE_HEIGHT 10
#define LIFE_WIDTH 10

char life[LIFE_HEIGHT][LIFE_WIDTH];

int main() {
	int sockfd, portno, n;		//создаем сокет
	struct sockaddr_in serv_addr;
	struct hostent *server;
      
	portno = 5001;
   
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
  
	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}
	
	server = gethostbyname("localhost");

	if (server == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
   
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);
   
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {	//коннект к серверу
		perror("ERROR connecting");
		exit(1);
	}
      
	bzero(life, LIFE_HEIGHT * LIFE_WIDTH);	//читаем из сокета
	n = read(sockfd, life, LIFE_HEIGHT * LIFE_WIDTH);

	int i,j;
	for (i = 0; i < LIFE_HEIGHT; i++) {
		for (j = 0; j < LIFE_WIDTH; j++) {
			printf("%c", life[i][j] == 0? ' ' : 'X');
		}
		printf("\n");
	}
}
