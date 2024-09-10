#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_LINE 80

void catCommand(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
    } else {
        char line[MAX_LINE];
        while (fgets(line, sizeof(line), file) != NULL) {
            printf("%s", line);
        }
        printf("\n");
        fclose(file);
    }
}
void execute_command(char *args[]) {
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipe_fd[0]);

        dup2(pipe_fd[1], STDOUT_FILENO);

        if (strcmp(args[0], "cat") == 0) {
            if (args[1] != NULL) {
                catCommand(args[1]);
            } else {
                fprintf(stderr, "Usage: cat <filename>\n");
            }
        } else if (!(strcmp(args[0], "exit") == 0)){
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        }

        exit(EXIT_SUCCESS);
    } else {
        close(pipe_fd[1]);

        char buffer[MAX_LINE];
        ssize_t bytes_read;

        while ((bytes_read = read(pipe_fd[0], buffer, sizeof(buffer))) > 0) {
            write(STDOUT_FILENO, buffer, bytes_read);
        }

        wait(NULL);

        close(pipe_fd[0]);
    }
}
int main(void) {
    char input[MAX_LINE];
    char *args[MAX_LINE/2 + 1];
    int should_run = 1;

    while (should_run) {
        printf("it007sh> ");
        fflush(stdout);

        fgets(input, sizeof(input), stdin);

        input[strcspn(input, "\n")] = '\0';

        int i = 0;
        char *token = strtok(input, " \n");
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " \n");
        }
        args[i] = NULL;

        execute_command(args);

        if (strcmp(args[0], "exit") == 0) {
            should_run = 0;
        }
    }

    return 0;
}