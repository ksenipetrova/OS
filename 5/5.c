#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PROC 1000

long long numbers[MAX_PROC];
int pipes [MAX_PROC];


void _sqrt (int in, int out) {
	long long number;

	fd_set set;
	FD_ZERO(&set);	//обнулили множество отслеживаемых дескрипторов
	FD_SET(in, &set);	//положили в него in

	struct timeval timeout;
	timeout.tv_sec = 5;	//таймаут селекта 5 секунд
	timeout.tv_usec = 0;

	select(in+1, &set, NULL, NULL, &timeout);	//ждем, пока можно будет читать из in

	FILE * fp = fdopen(in, "r");	// получаем входное число
	fscanf(fp, "%lld", &number);
	fclose(fp);

	long long root;		//считаем корень числа, если корень не целый, то root=-1
	if (number < 0) 
		root = -1;
	else {
		for (root = 0; root <= number; root++) {
			if (root*root == number)
				break;
			if (root*root > number) {
				root = -1;
				break;
			}
		}
	}
	printf("%lld\n", root);

	fp = fdopen(out, "w");	//подаем на выход посчитанное значение
	fprintf(fp, "%lld\n", root);
	fclose(fp);
}

void exec_sqrt(int i) {
	int p1 [2];
	pipe(p1);	//создаем две пайпы, чтобы общаться с процессом

	int p2 [2];
	pipe(p2);

	FILE * fp;
	
	int cpid = fork();

	switch (cpid) { 
	case -1:
		printf("Ошибка fork(); cpid == -1\n");
		exit(1);
	case 0:
		_sqrt(p1[0], p2[1]); // пайп1 - входной поток, пайп два - выходной, функция sqrt думает, что работает с файлами
		exit(0);
	default:
		fp = fdopen(p1[1], "w");	//в пайп1 пишем входные данные, отсюда процесс сын прочитает входные данные
		fprintf(fp, "%lld\n", numbers[i]);
		fclose(fp);
		pipes[i] = p2[0];	//в пайп2 будет подан результат, поэтому сохраняем, чтобы потом прочитать
	}
}

int main(int argc, char * argv[]) {
	FILE * fp = fopen("input", "r");

	if (fp == NULL) {
		printf("Не удалось открыть файл input\n");
	}
	
	int i = 0;
	int number;
	int numbers_len;
	while (fscanf(fp, "%lld", &number) == 1) {  //считываем входные числа
		numbers[i] = number;
		i++;
		if (i == MAX_PROC) {
			break;
		}
	}
	numbers_len = i;
	fclose(fp);

	for(i = 0; i < numbers_len; i++) {	//запускаем процессы для всех чисел
		exec_sqrt(i);
	}

	fp = fopen("output", "w");
	if (fp == NULL) {
		printf("Не удалось открыть файл output\n");
	}
	
	for(i = 0; i < numbers_len; i++) {
		fd_set set;
		FD_ZERO(&set);	//обнулили множество отслеживаемых дескрипторов
		FD_SET(pipes[i], &set);	//положили в него очередную пайпу

		struct timeval timeout;
		timeout.tv_sec = 30;	//таймаут селекта 30 секунд
		timeout.tv_usec = 0;

		select(pipes[i]+1, &set, NULL, NULL, &timeout);	//ждем, пока можно будет читать из пайпы

		FILE * pipe_fp = fdopen(pipes[i], "r");
	
		long long root;
		fscanf(pipe_fp, "%lld", &root);	//читаем ответ
		fclose(pipe_fp);
	
		if (root == -1) {
			fprintf(fp, "sqrt(%lld) не целое\n", numbers[i]);	//выводим в выходной файл
		} else {
			fprintf(fp, "sqrt(%lld) = %lld\n", numbers[i], root);
		}
	}

	fclose(fp);
}
