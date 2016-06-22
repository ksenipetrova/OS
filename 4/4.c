#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define MAX_FILE 100
#define MAX_LOGIN 100
#define MAX_PASS 100
#define MAX_LINES 1000

int main(int argc, char * argv[]) {
	if (argc != 4) {
		printf("Неправильные аргументы\n");
	}

	//argv[1] - имя файла
	//argv[2] - логин
	//argv[3] - пароль

	char lock[MAX_FILE+5];
	sprintf(lock, "%s.lck", argv[1]);

	while (access(lock, 0) != -1) {
		; // ждем разблокировки
	}
	
	FILE * lock_fp = fopen(lock, "w"); //блокируем файл

	fprintf(lock_fp, "%d:r", getpid()); //на чтение
	fclose(lock_fp);

	FILE * fp;
	fp = fopen(argv[1], "r");

	if (fp == NULL) {
		printf("Не удалось открыть файл");
		exit(1);
	}

	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	char * token;	
	const char delims[3] = " \n";
	char output [MAX_LINES*(MAX_LOGIN+MAX_PASS+2)];	//копим тут новое содержимое файла
	int pass_changed = 0;

	while ((read = getline(&line, &len, fp)) != -1) {
		char login [MAX_LOGIN];
		char pass [MAX_PASS];

		token = strtok(line, delims);
		strcpy(login, token);
		
		token = strtok(NULL, delims);
		strcpy(pass, token);

		if (strcmp(login, argv[2]) == 0) { //нашли такой логин
			sprintf(output, "%s%s %s\n", output, login, argv[3]);
			pass_changed = 1;
		} else {
			sprintf(output, "%s%s %s\n", output, login, pass);
		}
	}
	
	if(!pass_changed) { //то есть такого логина еще нет
		sprintf(output, "%s%s %s\n", output, argv[2], argv[3]);
		pass_changed = 1;
	}

	fclose(fp);

	lock_fp = fopen(lock, "w"); //блокируем файл на запись теперь

	fprintf(lock_fp, "%d:w", getpid());
	fclose(lock_fp);
	
	fp = fopen(argv[1], "w");
	
	fprintf(fp, "%s", output);
	fclose(fp);
	
	remove(lock); //убрали блокировку
}
