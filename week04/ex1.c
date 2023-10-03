#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
	clock_t start = clock();
	pid_t child1, child2;
	child1 = fork();
	if(child1 == 0){
		clock_t start = clock();
		printf("child-process:\n   id - %d\n   parent - id %d\n", getpid(), getppid());
		clock_t end = clock();
		printf("   time taken - %f\n", (double)(end-start));
	} else if(child1 > 0){
		child2 = fork();
		if(child2 == 0){
			clock_t start = clock();
			printf("child process:\n   id - %d\n   parent id - %d\n", getpid(), getppid());
			clock_t end = clock();
			printf("   time taken - %f\n", (double)(end-start));
		}
		else if(child2>0){
			wait(NULL);
			wait(NULL);
			printf("parent-process:\n   id - %d\n   parent id - %d\n", getpid(), getppid());
			clock_t end = clock();
			printf("   time taken - %f\n", (double)(end-start));
		} else{
			exit(1);
	}
	return 0;
}
