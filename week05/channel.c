#include <stdio.h>
#include <unistd.h>
int main(){
	pid_t subscriber;
	int pipefd[2];
	if(pipe(pipefd) == -1){
		exit(1);
	}
	subscriber = fork();
	if(subscriber == 0){
		close(pipefd[0]);
		char message[1024];
		scanf("%s", message);
		write(pipefd[1], message, sizeof(message));
		close(pipefd[1]);
	} else if(subscriber >0){
		close(pipefd[1]);
		char recieved[1024];
		read(pipefd[0], recieved, sizeof(recieved));
		printf("the message:\n%s\n", recieved);
	} else{
		exit(1);
	}
	return 0;
}

