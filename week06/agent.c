#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void handler(int signum){
	switch(signum){
	case SIGUSR1:
		FILE *text = fopen("text.txt", "r");
		char content[1024];
		fgets(content, 1024, text);
		printf("\n%s\n", content);
		break;
	case SIGUSR2:
		printf("Process terminating...\n");
		exit(0);
	}
}

int main(){
	printf("My pid: %d\n", getpid());
	printf("1\n");
	FILE *pidfile, *text;
	printf("2\n");
	pidfile = fopen("/var/run/agent.pid", "w");
	if(pidfile == NULL)
		printf("1234321234\n");
	printf("3\n");
	fprintf(pidfile,"%d", getpid());
	fclose(pidfile);
	printf("4\n");
	text = fopen("text.txt", "r");
	char content[1024];
	fgets(content, 1024, text);
	printf("%s\n", content);
	signal(SIGUSR1, &handler);
	signal(SIGUSR2, &handler);
	while(1);
	return 0;
}
