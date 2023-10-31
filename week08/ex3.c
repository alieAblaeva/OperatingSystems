#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <string.h>
#include <unistd.h>

int size = 100*1024*1024;

int main(){
	struct rusage usage;
	
	for(int i = 0; i<10;i++){
		char* addr = (char*)malloc(size);
		memset(addr, 0, size);
		printf("%d\n", getrusage(RUSAGE_SELF, &usage));
		sleep(1);
	}
	return 0;
}
