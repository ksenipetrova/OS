#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char * argv[]) {
	if (argc != 2) {
		printf("Неверные аргументы\n");
		exit(1);
	}

	int fd = open(argv[1], O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU);
	
	if (fd == -1) {
		exit(1);
	}

	char buff[1];
	int n = 0;
	long long len = 0;

	while(n = read(0, buff, 1)) {
		if (buff[0] == 0) {
			lseek(fd, 1, SEEK_CUR);
		} else {
			write(fd, buff, 1);
		}
		len++;
	}
	
	ftruncate(fd, len);

	close(fd);
}
