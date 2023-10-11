#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char**argv){
	int id = atoi(argv[1]);
	char number[20];
	sprintf(number, "%d", id);
	char filePath[30] = "/tmp/ex1/s";
	strcat(filePath, number);
	//mkfifo(filePath, 0666);
	printf("subscriber %d is opening the file\n", id);
	
	printf("subscriber opened the file\n");


	while(1){
		int fd = open(filePath, O_RDONLY);
		char message[1024];
		memset(message, 0, sizeof(message));
		read(fd, message, sizeof(message));
		close(fd);
		printf("%s", message);
	}
	return 0;
}
