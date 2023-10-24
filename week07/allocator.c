#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#define MAXSIZE 10000000

int heap[MAXSIZE] = {0};

void allocate_first_fit(unsigned int adrs, unsigned int size){
	int first = -1;
	int num = 0;
	for(int i= 0; i<MAXSIZE; i++){
		if(heap[i] == 0){
			if(first == -1){
				first = i;
			} 
			num++;
			if(num == size){
				for(int j = first; j<first+size; j++){
					heap[j] = adrs;
				}
				return;
			}
		} else{
			first = -1;
			num = 0;
		}
	}
}

void allocate_worst_fit(unsigned int adrs, unsigned int size){
	int maxFirst = -1;
	int maxNum = 0;
	int first;
	int num;
	for(int i = 0; i<MAXSIZE; i++){
		if(heap[i] == 0){
			if(first == -1){
				first = i;
			} 
			num++;
		} else{
			if(num>maxNum){
				maxNum = num;
				maxFirst = first;
			}
			first = -1;
			num = 0;
		}
	}
	for(int i = maxFirst; i<maxFirst+size; i++){
		heap[i] = adrs;
	}	
}

void allocate_best_fit(unsigned int adrs, unsigned int size){
	int minFirst = -1;
	int minNum = 0;
	int first;
	int num;
	for(int i = 0; i<MAXSIZE; i++){
		if(heap[i] == 0){
			if(first == -1){
				first = i;
			} 
			num++;
		} else{
			if(num<minNum){
				minNum = num;
				minFirst = first;
			}
			first = -1;
			num = 0;
		}
	}
	for(int i = minFirst; i<minFirst+size; i++){
		heap[i] = adrs;
	}	
}


void clear(unsigned int adrs){
	
	for(int i = 0; i<MAXSIZE; i++){
		if(heap[i] == adrs)
			heap[i] = 0;
	}
}


int main(){
	printf("beginning\n");
	FILE* queries = fopen("queries.txt", "r");
	if(queries == NULL){
		printf("Error opening file...\n");
		exit(EXIT_FAILURE);
	}
	printf("file opened successfully\n");
	char command[16];
	for(int i = 0; i<3; i++){
		if(i == 0)
			printf("allocate_first_fit allocator started working\n");
		if(i == 1)
			printf("allocate_worst_fit allocator started working\n");
		if(i == 2)
			printf("allocate_best_fit allocator started working\n");
		int numOfQueries = 0;

		clock_t start = clock();
		while(1){
			fscanf(queries, "%s", command);
			if(strcmp(command, "allocate" ) == 0){
				int adrs, size;
				fscanf(queries, "%d %d", &adrs, &size);
				if(i == 0)
					allocate_first_fit(adrs, size);
				else if (i == 1)
					allocate_worst_fit(adrs, size);
				else if(i ==2)
					allocate_best_fit(adrs, size);
			} else if(strcmp(command, "clear") == 0){
				int adrs;
				fscanf(queries, "%d", &adrs);
				clear(adrs);
			} else
				break;
			numOfQueries++;
		}
		clock_t finish = clock();
		float totalTime = (float)(finish-start)/CLOCKS_PER_SEC;
		printf("number of queries: %d\nallocation time: %f\nthroughput: %f\n",
			numOfQueries, totalTime, (float)numOfQueries/(float)(totalTime));
		memset(heap, 0, MAXSIZE*sizeof(heap[0]));
		rewind(queries);
	}
	fclose(queries);
	
	
	return 0;
}
