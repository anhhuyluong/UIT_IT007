#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include<string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length command */

int main(void)
{
    char args[MAX_LINE/2 + 1]; /* command line arguments */
    int should_run = 1; /* flag to determine when to exit program */
    while (should_run)
    {
        printf("it007sh>");
        fflush(stdout);
        // Read the input from the user
        fgets(args, MAX_LINE/2+1, stdin);
        // Remove the newline character
        args[strcspn(args, "\n")] = 0;
        if (strcmp(args, "exit") == 0)
        {
            exit(0);
        }
        char *pipe_symbol = strchr(args, '|');
        if (pipe_symbol != NULL)
        {

            char *first_cmd = strtok(args, "|");
            char *second_cmd = strtok(NULL, "|");
            // Create a pipe
            int fd[2];
            if (pipe(fd) == -1)
            {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
            // Fork the first child process
            pid_t pid1 = fork();
            if (pid1 == -1)
            {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            if (pid1 == 0)
            {
                // In the first child process
                // Redirect the standard output to the write end of the pipe
                dup2(fd[1], STDOUT_FILENO);
                // Close the unused read end of the pipe
                close(fd[0]);
                // Execute the first command
                system(first_cmd);
                // Exit the child process
                exit(EXIT_SUCCESS);
            }
            else
            {
                // In the parent process
                // Fork the second child process
                pid_t pid2 = fork();
                if (pid2 == -1)
                {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                if (pid2 == 0)
                {
                    // In the second child process
                    // Redirect the standard input to the read end of the pipe
                    dup2(fd[0], STDIN_FILENO);
                    // Close the unused write end of the pipe
                    close(fd[1]);
                    // Execute the second command
                    system(second_cmd);
                    // Exit the child process
                    exit(EXIT_SUCCESS);
                }
                else
                {
                    // In the parent process
                    // Close both ends of the pipe
                    close(fd[0]);
                    close(fd[1]);
                    // Wait for both child processes to finish
                    waitpid(pid1, NULL, 0);
                    waitpid(pid2, NULL, 0);
                }
            }
        }
        else
        {
            system(args);
        }
        
    }
    return 0;
}
