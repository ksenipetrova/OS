#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>

#define LIFE_HEIGHT 10
#define LIFE_WIDTH 10
char life[LIFE_HEIGHT][LIFE_WIDTH] = {
	0,1,0,0,0,0,0,0,0,0,
	0,0,1,0,0,0,0,0,0,0,
	1,1,1,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0
};
char oldLife [LIFE_HEIGHT][LIFE_WIDTH];


int countAdjusted(int i, int j) {
	int x, y;
	int adjusted_i, adjusted_j;
	int count = 0;
	for(x = -1; x < 2; x++) {	//перебор сдвигов
		for (y = -1; y < 2; y++) {
			if (x==0 && y==0)
				continue;
			adjusted_i = i+x;
			if (adjusted_i == -1)
				adjusted_i = LIFE_HEIGHT-1;
			if (adjusted_i == LIFE_HEIGHT)
				adjusted_i = 0;

			adjusted_j = j+y;
			if (adjusted_j == -1)
				adjusted_j = LIFE_WIDTH-1;
			if (adjusted_j == LIFE_WIDTH)
				adjusted_j = 0;
			
			count += oldLife[adjusted_i][adjusted_j];	//если жив, то будет +1
		}
	}

	return count;
}

void step() {
	int i,j;
	for (i = 0; i < LIFE_HEIGHT; i++) {	//копируем поле, чтобы на его основе менять текущее
		for (j = 0; j < LIFE_WIDTH; j++) {
			oldLife[i][j] = life[i][j];
		}
	}

	for (i = 0; i < LIFE_HEIGHT; i++) {
		for (j = 0; j < LIFE_WIDTH; j++) {
			switch(countAdjusted(i, j)) {
			case 3:		//при 3 живых хоть как живет
				life[i][j] = 1;
				break;
			case 2:		//при 2 либо остается мертвой, либо живой
				break;
			default:	//иначе умирает
				life[i][j] = 0;
			}	
		}
	}
}

void live() {	//тут каждую секунду считаем ход
	pthread_t step_t;
	while (1) {
		pthread_create(&step_t, NULL, &step, NULL); // сделали поток шага
		sleep(1);	//ждем секунду
		if (pthread_kill(step_t, NULL) == 0) {	//посылаем сигнал, чтобы проверить, существует ли еще поток step_t
			printf("За секунду не посчитали поле\n");
		}
		while (pthread_kill(step_t, NULL) == 0) { //ждем, пока закончит
		}
	}
}
 
int main () {
	pthread_t life_t;
	pthread_create(&life_t, NULL, &live, NULL); // сделали поток для жизни

	int sockfd, newsockfd, portno, clilen;		//настраиваем сервер
	struct sockaddr_in serv_addr, cli_addr;
	int  n;
   
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}
   
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = 5001;
   
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
   
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {	//собственно забиндили сервер
		perror("ERROR on binding");
		exit(1);
	}
      
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
   	
	while(1) {
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);	//приняли коннект

		int cpid = fork();	//сделаем дочерний процесс - обработчик

		switch (cpid) { 
		case -1:
			printf("Ошибка fork(); cpid == -1\n");
			exit(1);
		case 0:
			close(sockfd);
			n = write(newsockfd, life, LIFE_HEIGHT * LIFE_WIDTH);	//шлем поле
			close(newsockfd);
			exit(0);

		default:
			close(newsockfd);
		}

	   
	}	
}
