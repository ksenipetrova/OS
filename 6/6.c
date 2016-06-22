#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUM_LEN 1000

long long numbers [MAX_NUM_LEN];


int compare_long_long (const void * a, const void * b) 
{
    long long a_long = *((long long*)a);
    long long b_long = *((long long*)b);
    return a_long > b_long? 1: -1;
}

int main(int argc, char * argv[]) {
	FILE *fp;

	if (argc < 3) {
		printf("Неправильные аргументы\n");
	}

	long long numbers_len = 0;
	int i;

	for (i = 1; i < argc - 1; i++) {		//читаем все файлы, кроме последнего, в него писать
		fp = fopen(argv[i], "r");
	
		if (fp == NULL) {
			printf("Не удалось открыть файл %s\n", argv[i]);
			exit(1);
		}

		int n;
		char digit;
		long long number = -1;		//-1 будет означать, что число в данный момент не копится
		int j = 0;
		while ((n = fread(&digit, sizeof(char), 1, fp)) == 1) {	//находим числа
			if (digit >= '0' && digit <= '9') {
				if (number == -1) {
					number = digit - '0';	//начали копить
				} else {
					number = number * 10 + digit - '0'; //продолжили копить
				}
			} else {
				if (numbers_len == MAX_NUM_LEN) {
					printf("Чисел слишком много\n");
					exit(1);
				}
				if (number != -1) {
					numbers[numbers_len] = number;	//сохраняем число
					numbers_len++;
					number = -1;
				}
			}
		}

		if (number != -1) {
			if (numbers_len == MAX_NUM_LEN) {
				printf("Чисел слишком много\n");
				exit(1);
			}
			numbers[numbers_len] = number;	//сохраняем число
			numbers_len++;
			
		}
		fclose(fp);
	}

	qsort(numbers, numbers_len, sizeof(long long), compare_long_long);

	fp = fopen(argv[argc-1], "w");
	if (fp == NULL) {
		printf("Не удалось открыть файл %s\n", argv[argc-1]);
		exit(1);
	}

	for (i = 0; i < numbers_len; i++) {
		if (fprintf(fp, "%lld\n", numbers[i]) < 0) {
			printf("Не удалось записать в файл %s\n", argv[argc-1]);
			exit(1);
		}
	}
	fclose(fp);
	
	return 0;
}
