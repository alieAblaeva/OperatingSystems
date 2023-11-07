#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/types.h>

#define MAX_LINE 100

struct PTE {
    int valid;
    int frame;
    int dirty;
    int referenced;
    int counter_nfu;
};

struct TLB_entry {
    int page;
    int frame;
};

struct TLB_entry TLB[20]; 


int tlb_miss_count = 0;
int total_access_count = 0;

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <num_pages> <reference_string> <pager_pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_pages = atoi(argv[1]);
    //char *reference_string = argv[2];
    int pager_pid = atoi(argv[argc - 1]);

    int fd = open("/tmp/ex2/pagetable", O_RDWR, 0666);
    if (fd == -1) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    struct PTE *page_table = mmap(NULL, num_pages * sizeof(struct PTE), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (page_table == MAP_FAILED) {
        perror("Error mapping the file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < argc - 3; i++) {
        char mode = argv[i+2][0];
        int page = atoi(argv[i+2] + 1);
        char m[10];
        if(mode == 'R'){
        	strcpy(m, "Read");
        } else
        	strcpy(m, "Write");
        printf("%s Request for page %d\n", m, page);

        int tlb_hit = 0;
   
        for (int j = 0; j < 20; j++) {
            if (TLB[j].page == page) {
                page_table[page].referenced = getpid(); 
                printf("TLB Hit!\n");
                tlb_hit = 1;
                break;
            }
        }

        if (!tlb_hit) {
            tlb_miss_count++;
            printf("TLB Miss!\n");
            if (page_table[page].valid == 0) {
                printf("It is not a valid page --> page fault\n");
                printf("Ask pager to load it from disk (SIGUSR1 signal) and wait\n");
                printf("-------------------------\n");

                page_table[page].referenced = getpid();
                kill(pager_pid, SIGUSR1);
                kill(getpid(), SIGSTOP);
                printf("MMU resumed by SIGCONT signal from pager\n");
            } else
                printf("It is a valid page\n");

       
            for (int j = 19; j > 0; j--) {
                TLB[j] = TLB[j - 1];
            }
            TLB[0].page = page;
            TLB[0].frame = page_table[page].frame;
        }

        if (mode == 'W') {
            page_table[page].dirty = 1;
        }

        printf("Page table\n");
        for (int j = 0; j < num_pages; j++) {
            printf("Page %d ---> valid=%d, frame=%d, dirty=%d, referenced=%d\n", j, page_table[j].valid,
                   page_table[j].frame, page_table[j].dirty, page_table[j].referenced);
        }
        printf("-------------------------\n");
        printf("\n");

        total_access_count++;
    }

    close(fd);
    printf("MMU sends SIGUSR1 to the pager.\n");
    kill(pager_pid, SIGUSR1);
    printf("MMU terminates.\n");

    double tlb_miss_ratio = (double)tlb_miss_count / total_access_count;
    printf("TLB miss ratio: %lf\n", tlb_miss_ratio);

    return 0;
}

