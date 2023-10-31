#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <ctype.h>

#define PASSWORD_LENGTH 13

int main() {
    FILE* myPid = fopen("/tmp/ex1.pid", "w");
    if (myPid == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int pid = getpid();
    printf("pid %d\n", pid);
    fprintf(myPid, "%d", pid);
    fclose(myPid);

    int randomData = open("/dev/urandom", O_RDONLY);
    if (randomData < 0) {
        perror("Error opening /dev/urandom");
        exit(EXIT_FAILURE);
    }

    char password[PASSWORD_LENGTH + 1] = "pass:";

    for (int i = 5; i < PASSWORD_LENGTH; i++) {
        char symbol;
        while (1) {
            read(randomData, &symbol, sizeof(char));
            if (isprint(symbol) && symbol != '\'' && symbol != '\\') {
                password[i] = symbol;
                break;
            }
        }
    }
    password[PASSWORD_LENGTH] = '\0'; 
    printf("Password generated: %s\n", password);

    char *mapped_password = mmap(NULL, (PASSWORD_LENGTH + 1) * sizeof(char), 
                                 PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (mapped_password == MAP_FAILED) {
        perror("Error mapping memory");
        exit(EXIT_FAILURE);
    }

    strcpy(mapped_password, password);

    while (1);
    munmap(mapped_password, (PASSWORD_LENGTH + 1) * sizeof(char));
    return 0;
}

