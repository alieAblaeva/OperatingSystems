#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#define MAX_REFERENCES 100

int number_of_pages;
int number_of_frames;
int alg = -1;

struct PTE {
    bool valid;
    int frame;
    bool dirty;
    int referenced;
    int counter_nfu;
    unsigned char reference_bits;
};

struct PTE *page_table;
int disk_accesses = 0;
char** RAM;
char** disk;

int whichPage(struct PTE *table, int frame, int n){
	for(int i = 0; i<n; i++)
		if(table[i].frame == frame)
			return i;
}

int randomm(struct PTE* page_table, int num, int pages){
	int frame = rand() % num;
	int page = whichPage(page_table, frame, pages);
	return page;
}

int nfu(struct PTE* page_table){
	int least = 100000;
	int index;
	for(int i = 0; i<number_of_pages; i++){
		if(page_table[i].counter_nfu < least && page_table[i].valid == 1){
			printf("%d-------\n", page_table[i].valid);
			least = page_table[i].counter_nfu;
			index = i;
		}
	}
	printf("%d++++++++n", page_table[index].valid);
	return index;
}

int aging(struct PTE* page_table){
	int least = 100000;
	int index;
	for(int i = 0; i<number_of_pages; i++){
		if(page_table[i].reference_bits < least && page_table[i].valid == 1){
			least = page_table[i].reference_bits;
			index = i;
		}
	}
	return index;
}

void copy(char dest[8], char sour[8]){
	for(int i = 0; i<8; i++){
		dest[i] = sour[i];
	}
}

void handle_SIGUSR1(int sig) {
    sleep(1);
    for (int i = 0; i < number_of_pages; i++) {
        if (page_table[i].referenced != 0) {
            int pid = page_table[i].referenced;
            printf("A disk access request from MMU Process (pid=%d)\n", pid);
            printf("Page %d is referenced\n", i);
            if (!page_table[i].valid) {
                int free_frame = -1;
                for (int j = 0; j < number_of_frames; j++) {
                    if (strcmp(RAM[j], "") == 0) {
                        free_frame = j;
                        break;
                    }
                }
                if (free_frame != -1) {
                    copy(RAM[free_frame], disk[i]);
                    page_table[i].valid = true;
                    page_table[i].frame = free_frame;
                    printf("We can allocate it to free frame %d\n", free_frame); 
                    printf("Copy data from the disk (page=%d) to RAM (frame=%d)\n", i, free_frame);
                    printf("RAM array\n");
                    for (int k = 0; k < number_of_frames; k++) {
                        printf("Frame %d ---> %s\n", k, RAM[k]);
                    }
                    disk_accesses++;
                    
                } else {
                    printf("We do not have free frames in RAM\n");
                    srand(time(0));
                    int victim_page =-1;
                    if(alg == 1)
	                    victim_page = randomm(page_table, number_of_frames, number_of_pages);
	            else if(alg == 2)
	            	    victim_page = nfu(page_table);
	            else if(alg == 3)
	            	    victim_page = aging(page_table);
                    printf("Chose a random victim page %d\n", victim_page);
                    if (page_table[victim_page].dirty) {
                        copy(disk[victim_page], RAM[page_table[victim_page].frame]);
                        printf("Copy data from RAM (frame=%d) to disk (page=%d)\n", page_table[victim_page].frame, victim_page);
                    }
                    disk_accesses++;
                    copy(RAM[page_table[victim_page].frame], disk[i]);
                    printf("Replace/Evict it with page %d to be allocated to frame %d\n", i, page_table[victim_page].frame);
                    int vict = page_table[victim_page].frame;
                    page_table[victim_page].valid = false;
                    page_table[victim_page].dirty = false;
                    page_table[victim_page].frame = -1;
                    page_table[i].valid = true;
                    page_table[i].frame = vict;
                    printf("Copy data from the disk (page=%d) to RAM (frame=%d)\n", i, page_table[victim_page].frame);
                    printf("RAM array\n");
                    for (int k = 0; k < number_of_frames; k++) {
                        printf("Frame %d ---> %s\n", k, RAM[k]);
                    }
                }
                printf("disk access is %d so far\n\n", disk_accesses);
            }
            for(int i = 0; i<number_of_pages; i++){
            	page_table[i].referenced = 0;
            }
           
            kill(pid, SIGCONT);
            return;
        }

    }
    printf("%d disk accesses in total\n", disk_accesses);
    
    printf("Pager is terminated\n");
    munmap(page_table, sizeof(struct PTE) * number_of_pages);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <number_of_pages> <number_of_frames>\n", argv[0]);
        exit(1);
    }

    number_of_pages = atoi(argv[1]);
    number_of_frames = atoi(argv[2]);
    if(strcmp(argv[3], "random") == 0)
    	alg = 1;
    else if(strcmp(argv[3], "nfu") == 0)
    	alg = 2;
    else if(strcmp(argv[3], "aging") == 0)
    	alg = 3;
    RAM = (char**)malloc(number_of_frames*sizeof(char*));
    for(int i = 0; i<number_of_frames; i++){
    	RAM[i] = (char*)malloc(number_of_frames*sizeof(char));
    }
    
    disk = (char**)malloc(number_of_pages*sizeof(char*));
    for(int i = 0; i<number_of_pages; i++){
    	disk[i] = (char*)malloc(number_of_pages*sizeof(char));
    }
    
    if (number_of_frames > number_of_pages) {
        fprintf(stderr, "Error: Number of frames cannot be greater than the number of pages.\n");
        exit(1);
    }
    

    int fd = open("/tmp/ex2/pagetable", O_CREAT | O_RDWR, 0666);
    ftruncate(fd, sizeof(struct PTE) * number_of_pages);
    page_table = (struct PTE*)mmap(NULL, sizeof(struct PTE) * number_of_pages, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    //printf("pager process\n");
    printf("-------------------------\n");
    printf("Initialized page table\n");
    printf("Page Table\n");
    for (int i = 0; i < number_of_pages; i++) {
        page_table[i].valid = false;
        page_table[i].frame = -1;
        page_table[i].dirty = false;
        page_table[i].referenced = 0;
        printf("Page %d ---> valid=%d, frame=%d, dirty=%d, referenced=%d\n", i, page_table[i].valid, page_table[i].frame, page_table[i].dirty, page_table[i].referenced);
    }
    printf("-------------------------\n");
    printf("Initialized RAM\n");
    printf("RAM array\n");
    for (int i = 0; i < number_of_frames; i++) {
    	strcpy(RAM[i], "");
        printf("Frame %d --->\n", i);
    }
    printf("-------------------------\n");
    printf("Initialized disk\n");
    printf("Disk array\n");
    for (int i = 0; i < number_of_pages; i++) {
    	for(int j= 0; j<8; j++){
		disk[i][j] = rand() % 26 + 'a';
	}
        printf("Page %d ------> %s\n", i, disk[i]);
    }
    printf("-------------------------\n");

    signal(SIGUSR1, handle_SIGUSR1);
    while (1) {
        pause();
    }

    return 0;
}

