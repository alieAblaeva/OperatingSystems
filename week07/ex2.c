#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/mman.h>
#include <math.h>


int main(){
	long sz = 1024*sysconf(_SC_PAGESIZE);
	printf("%ld\n", sz);
	int size = 4*1024*1024;
	FILE* text = fopen("text.txt", "w");
	int randomData = open("/dev/random", O_RDONLY);
	if(randomData < 0){
		printf("Error opening /dev/random\n");
		exit(EXIT_FAILURE);
	}
	char symbol;
	int i = 0;
	while(i<size){
		if(i%1024 == 0 && i !=0){
			fprintf(text, "\n");
			i++;
		}
		read(randomData, &symbol, sizeof(char));
		if(isprint(symbol)){
			fprintf(text, "%c", symbol);
			i++;
		}
		
	}
	fclose(text);
	int text1 = open("text.txt", O_RDWR);
	int numOfChunks = size/sz;
	i = 0;
	int upper = 0;
	FILE* newText = fopen("newText.txt", "w");
	while(i<numOfChunks){
		int upperInChunk = 0;
		char *map = mmap(0, sz, PROT_READ | PROT_WRITE, MAP_SHARED, text1, i*sz);
		if(map == MAP_FAILED)
			printf("Error creating map\n");
		for(int j = 0; j<sz; j++){
			if(isupper(map[j])){
				upperInChunk++;
				map[j] = tolower(map[j]);
			}
		}
		upper+=upperInChunk;
		i++;
	}
		
	
	printf("In total %d capital letters\n", upper);
	
	
	return 0;
}
