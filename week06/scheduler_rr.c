#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#define PS_MAX 10


typedef struct{
	int idx; 
	int at, bt, rt, wt, ct, tat; 
	int burst; 
} ProcessData;

int running_process = -1; 

unsigned total_time; 

ProcessData data[PS_MAX]; 
int* sorted_idx;

pid_t ps[PS_MAX]; 

int quantum;

unsigned data_size;

void read_file(FILE* file){
	data_size = 0;
	char header[16];
	fgets(header, 16, file);
	char info[256];
	while(fgets(info, sizeof(info), file)){
		int idx, at, bt;
		sscanf(info, "%d %d %d", &idx, &at, &bt);
		data[data_size].idx = idx;
		data[data_size].at = at;
		data[data_size].bt = bt;
		data[data_size].burst = bt;
		data_size++;
	}
	for (int i = 0; i<data_size; i++){
		ps[i] = 0;
	}
	sorted_idx = (int*)malloc(data_size*sizeof(int));
	for(int i = 0; i<data_size; i++){
		sorted_idx[i] = data[i].idx;
	}
	for(int i = 0; i<data_size-1; i++){
		for(int j = i+1; j<data_size; j++){
			if(data[sorted_idx[i]].at>data[sorted_idx[j]].at){
				int temp = sorted_idx[i];
				sorted_idx[i] = sorted_idx[j];
				sorted_idx[j] = temp;
			}
			
		}
	}
}

void resume(pid_t process) {
    if(process!=0){
    	kill(process, SIGCONT);
    }
}

void suspend(pid_t process) {
    if(process!=0){
    	kill(process, SIGTSTP);
    }
}

void terminate(pid_t process) {
    if(process!=0){
    	kill(process, SIGTERM);
    }
}

void create_process(int new_process) {
	
	if(running_process != -1){
		suspend(ps[running_process]);
	}
	pid_t newpid= fork();


	if(newpid>0){
		running_process = new_process;
		ps[new_process] = newpid;
		
		}

    	if(newpid == 0){
    		char number[256];
    		sprintf(number, "%d", new_process);
    		char*args[] = {"./worker", number, NULL};
		execvp(args[0], args);
	}

}

static int location = 0;

ProcessData find_next_process() {

    int starting_point = location;
    do {
        int next = (location + 1) % data_size;
        if (data[sorted_idx[next]].at <= total_time && data[sorted_idx[next]].burst > 0) {
            location = next;
            return data[sorted_idx[location]];
        }
        location = (location + 1) % data_size;
    } while (location != starting_point);
    printf("Scheduler: Runtime: %u seconds.\nProcess: has not arrived yet.\n", total_time);
    total_time++;
    return find_next_process();
}


void report(){
	printf("Simulation results.....\n");
	int sum_wt = 0;
	int sum_tat = 0;
	for (int i=0; i< data_size; i++){
		printf("process %d: \n", i);
		printf("	at=%d\n", data[i].at);
		printf("	bt=%d\n", data[i].bt);
		printf("	ct=%d\n", data[i].ct);
		printf("	wt=%d\n", data[i].wt);
		printf("	tat=%d\n", data[i].tat);
		printf("	rt=%d\n", data[i].rt);
		sum_wt += data[i].wt;
		sum_tat += data[i].tat;
	}

	printf("data size = %d\n", data_size);
	float avg_wt = (float)sum_wt/data_size;
	float avg_tat = (float)sum_tat/data_size;
	printf("Average results for this run:\n");
	printf("	avg_wt=%f\n", avg_wt);
	printf("	avg_tat=%f\n", avg_tat);
}

void check_burst(){

	for(int i = 0; i < data_size; i++){
		if (data[i].burst > 0)
			return;
	}

    report();


	  exit(EXIT_SUCCESS);
}

int time_left = 0;

void schedule_handler(int signum) {
    ProcessData next_process;
    total_time++;

    if (running_process != -1 && time_left!=0) {
        data[running_process].burst -= 1;
        time_left -= 1;
        printf("Scheduler: Runtime: %d seconds\nScheduler: Process %d is running with %d seconds left\n", total_time, running_process, data[running_process].burst);

        if (data[running_process].burst == 0) {
            time_left = 0;
            terminate(ps[running_process]);
            printf("Scheduler: Terminating Process %d (Remaining Time: %d)\n", running_process, data[running_process].burst);
            waitpid(ps[running_process], NULL, 0);
            data[running_process].ct = total_time;
            data[running_process].tat = data[running_process].ct - data[running_process].at;
            data[running_process].wt = data[running_process].tat - data[running_process].bt;
            running_process = -1;
        }
    }

    check_burst();
    if (time_left == 0) {
        time_left = quantum;
        next_process = find_next_process();
        if (running_process != -1) {
            suspend(ps[running_process]);
            printf("Scheduler: Scheduler:  Stopping Process %d (Remaining Time: %d)\n", running_process, data[running_process].burst);
        }

        running_process = next_process.idx;
        if (ps[running_process] == 0) {
            printf("Scheduler: Starting Process %d (Remaining Time: %d)\n", running_process, data[running_process].burst);
            create_process(running_process);
            data[running_process].rt = total_time - data[running_process].at;
        } else {
            resume(ps[running_process]);
            printf("Scheduler: Resuming Process %d (Remaining Time: %d)\n", running_process, data[running_process].burst);
        }
    }
}




int main(int argc, char *argv[]) {
	
    printf("quantum = ");
    scanf("%d", &quantum);

    FILE *in_file  = fopen(argv[1], "r");
  	if (in_file == NULL) {   
		printf("File is not found or cannot open it!\n"); 
  		exit(EXIT_FAILURE);
    } else {
        read_file(in_file);  
    }

    // set a timer
    struct itimerval timer;

    // the timer goes off 1 second after reset
    timer.it_value.tv_sec = 1;
    timer.it_value.tv_usec = 0;

    // timer increments 1 second at a time
    timer.it_interval.tv_sec = 1;
    timer.it_interval.tv_usec = 0;

    // this counts down and sends SIGALRM to the scheduler process after expiration.
    setitimer(ITIMER_REAL, &timer, NULL);

    // register the handler for SIGALRM signal
    signal(SIGALRM, schedule_handler);

    // Wait till all processes finish
    while(1); // infinite loop
}
