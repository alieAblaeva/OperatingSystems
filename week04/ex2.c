#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int iterator = 0;

int main(){
	FILE *results;
	results = fopen("temp.txt", "w+");
	int range = 100;
	srand(time(NULL));
	int u[120];
	int v[120];
	for(int i = 0; i<120; i++){
		u[i] = rand()%range;
		v[i] = rand()%range;
	}
	
	
	int n;
	scanf("%d", &n);
	pid_t processes[n];
	for(int i =0; i<n; i++){
		processes[i] = fork();
		if(processes[i] == 0){
			int result = 0;
			if(i!=n-1){
				for(int j = i*120/n; j<(i+1)*120/n; j++){
					result += u[j]*v[j];
				}
			 } else{
				for(int j = i*120/n; j<=120; j++){
					result += u[j]*v[j];
				}
			}
			if(results!=NULL){
				fprintf(results,"%d\n", result);
			}
			fclose(results);
			exit(0);
		}	
		else{
			wait(NULL);
		}
	}
	rewind(results);
	int finResult = 0;
	for(int i= 0; i<n; i++){
		int res;
		if(results != NULL){
			fscanf(results, "%d", &res);
			finResult += res;
		}
	}
	printf("The dot producg for vectors u and v is %d\n", finResult);
	fclose(results);
	
	return 0;
}
