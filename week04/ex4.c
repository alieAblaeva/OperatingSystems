#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

void execute_command(char *command) {
    char *args[MAX_ARGS];
    int arg_count = 0;

    char *token = strtok(command, " \t\n");
    while (token != NULL) {
        args[arg_count++] = token;
        token = strtok(NULL, " \t\n");
        }
    args[arg_count] = NULL;

    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        
        if (execvp(args[0], args) == -1) {
            perror("Execvp failed");
            exit(EXIT_FAILURE);
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

int main() {
    char input[MAX_INPUT_SIZE];

    while (1) {
        printf("shell: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Error reading input");
            exit(EXIT_FAILURE);
        }

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0) {
            break;
        }

        execute_command(input);
    }

    return 0;
}
