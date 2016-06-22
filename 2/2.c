#include <stdio.h>
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>

#define MAX_PROC 100
#define MAX_NAME 100
#define MAX_ARGS 100
#define MAX_ARGS_LEN 100

char name[MAX_PROC][MAX_NAME];	//имена запускаемых файлов
char * args[MAX_PROC][MAX_ARGS_LEN + 1];	//аргументы запускаемых файлов
int respawnable[MAX_PROC];	//будет ли процесс перезапускаться
pid_t pid_list[MAX_PROC];		//айди процессов

int proc_count = 0;

int createFile(int i) {
	char file[MAX_NAME+MAX_PROC+10];
	sprintf(file, "/tmp/%i%s", i, name[i]);

	int j = 6;
	while(file[j] != 0) {
		if (file[j] == '/') {	//заменяем, так как иначе восприниматься будут как путь
			file[j] = '-';
		}
		j++;
	}

	FILE * fp = fopen(file, "w");

	if (fp == NULL) {
		printf("Не удалось открыть файл /tmp/%i%s\n", i, name[i]);
		exit(1);
	}

	fprintf(fp, "%i", pid_list[i]);

	fclose(fp);
}

int removeFile(int i) {
	char file[MAX_NAME+MAX_PROC+10];
	sprintf(file, "/tmp/%i%s", i, name[i]);

	int j = 6;
	while(file[j] != 0) {
		if (file[j] == '/') {	//заменяем, так как иначе восприниматься будут как путь
			file[j] = '-';
		}
		j++;
	}

	if (remove(file) != 0) {
		printf("Не удалось удалить файл /tmp/%i%s\n", i, name[i]);
		exit(1);
	}
}

int spawn(int i) {
	int cpid = fork();

	switch (cpid) { 
	case -1:
		printf("Ошибка fork(); cpid == -1\n");
		return -1;
		break;
	case 0:		//в потомке запускаем прогу
		printf("%d spawned, pid = %d\n", i, cpid);
		cpid = getpid();
		execvp(name[i], args[i]);
    		printf("Exit child %d, pid = %d\n", i, getpid());
		exit(0);
	default:	//в родителе запоминаем пид
		pid_list[i] = cpid;
		createFile(i);
		proc_count++;

		return 0;
	}
}

int main(int argc, char * argv []) {

	if (argc != 2) {
		printf("Неправильные аргументы\n");
		exit(1);
	}	

	FILE * fp;

	fp = fopen(argv[1], "r");

	if (fp == NULL)
		exit(1);

	int i = 0;

	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	char * token;	
	const char delims[4] = ", \n";

	while ((read = getline(&line, &len, fp)) != -1 && i < MAX_PROC) {	//в каждой строке описание запуска, имя, тип и аргументы
		token = strtok(line, delims);
		strcpy(name[i], token);
		
		token = strtok(NULL, delims);
		if (strcmp(token, "wait") == 0) {
			respawnable[i] = 0;
		} else {
			respawnable[i] = 1;
		}


		int j = 0;
		while((token = strtok(NULL, delims)) && j < MAX_ARGS_LEN) {
			args[i][j] = (char*) malloc(MAX_ARGS);
			strcpy(args[i][j], token);
			j++;
		}

		args[i][j] = NULL;

		i++;
	}
	
	int proc_number = i;

	fclose(fp);

	for (i = 0; i < proc_number; i++) {
		spawn(i);	//запускаем все процессы
	}


	while (proc_count) {
	    	pid_t cpid = waitpid(-1, NULL, 0);   //ждем любого завершенного потомка
		
		int i;
	    	for (i = 0; i < proc_number; i++) {
			if (pid_list[i] == cpid) {
		            	printf("Child number %d pid %d finished\n",i,cpid);
			    	proc_count--;
				removeFile(i);

				if (respawnable[i]) {	//если перезагружаемый, то снова запустим
					spawn(i);
				}
			}
		}
	}

	return;
}
