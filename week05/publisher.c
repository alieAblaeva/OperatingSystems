#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char**argv){
	int n = atoi(argv[1]);
	char filepaths[n][30];
	for(int i = 0; i<n; i++){
		char number[20];
		sprintf(number, "%d", i);
		char filePath[30] = "/tmp/ex1/s";
		strcat(filePath, number);
		strcpy(filepaths[i], filePath);
		//if (access(filePath, F_OK) == 0) 
            	//	remove(filePath) != 0;
		mkfifo(filePath, 0666);
	}

	char message[1024];

	while(fgets(message, sizeof(message), stdin) != NULL){
		
		//if(fgets(message, sizeof(message), stdin) == NULL)
			//break;
			//return 0;
		//scanf("%s", message);
		for(int i = 0; i<n; i++){
			if(fork()==0){
				printf("publisher %d is opening fifo...\n", i);
				int fd = open(filepaths[i], O_WRONLY);
				printf("publisher %d opened successfully\n", i);
				printf("publisher %d is writing to file\n", i);
				if(write(fd, message, strlen(message)) == -1){
					exit(1);
				}
				printf("publisher %d wrote successfully\n", i);
				close(fd);
				exit(0);
			}
			wait(NULL);
		}
		
		
	}
	return 0;
}
