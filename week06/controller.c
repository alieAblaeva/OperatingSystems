#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

pid_t agentPID;

void handler(int signum){
	kill(agentPID, SIGINT);
	exit(0);
}
	
int main(){
	FILE *pidfile;
	pidfile = fopen("/var/run/agent.pid", "r");
	
	if(fscanf(pidfile, "%d", &agentPID) == 0){
		printf("Error: No agent found.\n");
		exit(1);
	}
	printf("Agent found.\n");
	printf("PID: %d\n", agentPID);
	char command[16];
	signal(SIGINT, handler);
	while(1){
		printf("Choose a command {\"read\", \"exit\", \"stop\", \"continue\"} to send to the agent\n");
		scanf("%s", command);
		if(strcmp(command, "read") == 0){
			kill(agentPID, SIGUSR1);
		}
		else if(strcmp(command, "exit") == 0){
			kill(agentPID, SIGUSR2);
		}
		else if(strcmp(command, "stop") == 0){
			kill(agentPID, SIGSTOP);
			}
		else if(strcmp(command, "continue") == 0)
			kill(agentPID, SIGCONT);
		sleep(1);
	}
	return 0;
}
