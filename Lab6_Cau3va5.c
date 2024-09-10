#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#define MAXLINE 80 /* The maximum length command */

int should_run = 1; /* flag to determine when to exit program */

void sigintHandler(int sig_num) {
    signal(SIGINT, sigintHandler);
    printf("\nit007sh> ");
    fflush(stdout);
}

int main(void) {
    signal(SIGINT, sigintHandler); // Set up SIGINT handler

    char *args[MAXLINE/2 + 1]; /* command line arguments */

    while (should_run) {
        printf("it007sh> ");
        fflush(stdout);

        char input[MAXLINE]; // Buffer to store user input
        fgets(input, MAXLINE, stdin);

        char *token; // Token to split input by space
        int i = 0; // Argument index

        token = strtok(input, " \n"); // Tokenize the input
        while (token != NULL) {
            args[i] = token; // Store token as an argument
            token = strtok(NULL, " \n");
            i++;
        }
        args[i] = NULL; // Set the last argument to NULL for execvp

        // Check for redirection operators
        int input_redir = 0, output_redir = 0;
        char *input_file = NULL, *output_file = NULL;

        for (int j = 0; j < i; j++) {
            if (strcmp(args[j], "<") == 0) {
                input_redir = 1;
                input_file = args[j + 1];
                args[j] = NULL; // Set the '<' to NULL for execvp
            } else if (strcmp(args[j], ">") == 0) {
                output_redir = 1;
                output_file = args[j + 1];
                args[j] = NULL; // Set the '>' to NULL for execvp
            }
        }

        // Fork a child process
        pid_t pid = fork();

        if (pid < 0) {
            printf("Fork failed\n");
            return 1;
        } else if (pid == 0) { // Child process
            if (input_redir) {
                int fd0 = open(input_file, O_RDONLY);
                dup2(fd0, STDIN_FILENO);
                close(fd0);
            }

            if (output_redir) {
                int fd1 = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                dup2(fd1, STDOUT_FILENO);
                close(fd1);
            }

            // Execute the command
            execvp(args[0], args);
            exit(0);
        } else { // Parent process
            wait(NULL);
        }
    }

    return 0;
}