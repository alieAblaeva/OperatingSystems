#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char**argv){
	char* num = argv[1];
	int n = atoi(num);
	for(int i = 0; i<n; i++){
		pid_t child = fork();
		sleep(5);
	}
	printf("process with id %d\n", getpid());
	return 0;
}
