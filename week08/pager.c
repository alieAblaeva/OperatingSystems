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

struct PTE {
    bool valid;
    int frame;
    bool dirty;
    int referenced;
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

void copy(char dest[8], char sour[8]){
	for(int i = 0; i<8; i++){
		dest[i] = sour[i];
	}
}

void handle_SIGUSR1(int sig) {
    sleep(1);
    for (int i = 0; i < 4; i++) {
        if (page_table[i].referenced != 0) {
            int pid = page_table[i].referenced;
            printf("A disk access request from MMU Process (pid=%d)\n", pid);
            printf("Page %d is referenced\n", i);
            if (!page_table[i].valid) {
                int free_frame = -1;
                for (int j = 0; j < 2; j++) {
                    if (strcmp(RAM[j], "") == 0) {
                        free_frame = j;
                        break;
                    }
                }
                if (free_frame != -1) {
                    copy(RAM[free_frame], disk[i]);
                    page_table[i].valid = true;
                    page_table[i].frame = free_frame;
                    page_table[i].referenced = 0;
                    printf("We can allocate it to free frame %d\n", free_frame); 
                    printf("Copy data from the disk (page=%d) to RAM (frame=%d)\n", i, free_frame);
                    printf("RAM array\n");
                    for (int k = 0; k < 2; k++) {
                        printf("Frame %d ---> %s\n", k, RAM[k]);
                    }
                    disk_accesses++;
                    
                } else {
                    printf("We do not have free frames in RAM\n");
                    srand(time(0));
                    int victim_frame = rand() % 2;
                    int victim_page = whichPage(page_table, victim_frame, 4);
                    printf("Chose a random victim page %d\n", victim_page);
                    if (page_table[victim_page].dirty) {
                        
                        copy(disk[victim_page], RAM[victim_frame]);
                        printf("Copy data from RAM (frame=%d) to disk (page=%d)\n", victim_frame, victim_page);
                    }
                    disk_accesses++;
                    copy(RAM[victim_frame], disk[i]);
                    printf("Replace/Evict it with page %d to be allocated to frame %d\n", i, victim_frame);
                    
                    page_table[victim_page].valid = false;
                    page_table[victim_page].dirty = false;
                    page_table[victim_page].referenced = 0;
                    page_table[victim_page].frame = -1;
                    page_table[i].valid = true;
                    page_table[i].frame = victim_frame;
                    page_table[i].referenced = 0;
                    printf("Copy data from the disk (page=%d) to RAM (frame=%d)\n", i, victim_frame);
                    printf("RAM array\n");
                    for (int k = 0; k < 2; k++) {
                        printf("Frame %d ---> %s\n", k, RAM[k]);
                    }
                }
                printf("disk access is %d so far\n\n", disk_accesses);
            }
           
            kill(pid, SIGCONT);
            return;
        }

    }
    printf("%d disk accesses in total\n", disk_accesses);
    
    printf("Pager is terminated\n");
    munmap(page_table, sizeof(struct PTE) * 4);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number_of_pages> <number_of_frames>\n", argv[0]);
        exit(1);
    }

    int number_of_pages = atoi(argv[1]);
    int number_of_frames = atoi(argv[2]);
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

