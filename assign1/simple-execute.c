/* ------------------------------
 * | Name: SZE-TO Kwok Leung
 * | SID: 1155149068
 * ------------------------------
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define MAX_PIPE_COUNT 10

int shell_execute(char **args, int argc)
{
    // Quit
    if (strcmp(args[0], "EXIT") == 0)
        return -1;

    // Pipe
    int pipe_count = 0;
    int pipe_indecies[MAX_PIPE_COUNT];
    int pipefds[MAX_PIPE_COUNT][2];

    int i;

    // Initialize
    for (int i = 0; i < MAX_PIPE_COUNT; i++)
    {
        pipefds[i][0] = 0;
        pipefds[i][1] = 0;
        pipe_indecies[i] = 0;
    }

    // Parse pipe
    for (i = 0; i < argc - 1 && pipe_count < MAX_PIPE_COUNT; i++)
    {
        if (strcmp(args[i], "|") == 0)
        {
            pipe_count++;
            args[i] = NULL;
            pipe_indecies[pipe_count] = i + 1;
        }
    }

    int child_pid, wait_return, status;

    if ((child_pid = fork()) < 0)
    {
        // Error
        perror("Error while forking main process \n");
    }
    else if (child_pid == 0)
    {
        int child_pid = -1, wait_return = -1, status = -1;

        // Create child processes and execute commands
        for (i = 0; i < pipe_count; i++)
        {
            // Create pipe
            if (pipe(pipefds[i]) < 0)
            {
                perror("Error in pipe() \n");
                exit(EXIT_FAILURE);
            }

            if ((child_pid = fork()) < 0)
            {
                // Error
                perror("Error in fork() \n");
                exit(EXIT_FAILURE);
            }
            else if (child_pid == 0)
            {
                // Child process
                // ! Do nothing and countinue to next loop
            }
            else
            {
                // Parent process
                // ! Jump out of the loop to execute the command
                break;
            }
        }

        // ? Wait for the commands (child processes) to finish execute
        if (child_pid > 0)
        {
            if ((wait_return = wait(&status)) < 0)
            {
                perror("Error in wait() of command pipe\n");
                exit(EXIT_FAILURE);
            }
        }

        printf("? Process %d is executing command %s \n", i, args[pipe_indecies[pipe_count - i]]);

        // Redirect stdin to get the ouput from the previous command (output of the child process)
        if (i < pipe_count)
        {
            if (dup2(pipefds[i][0], STDIN_FILENO) < 0)
            {
                perror("Error in dup2(STDIN_FILENO) \n");
                exit(EXIT_FAILURE);
            }
        }

        if (i > 0)
        {
            // Redirect stdout to send the ouput to the next command (parent this process)
            if (dup2(pipefds[i - 1][1], STDOUT_FILENO) < 0)
            {
                perror("Error in dup2(STDOUT_FILENO) \n");
                exit(EXIT_FAILURE);
            }
        }

        // Execute command
        if (execvp(args[pipe_indecies[pipe_count - i]], args + pipe_indecies[pipe_count - i]) < 0)
        {
            perror("Error in execvp() \n");
            exit(EXIT_FAILURE);
        }

        // Finish executing a command
    }
    else
    {
        // Main Parent process
        // Wait for the commands to finish execute
        if ((wait_return = wait(&status)) < 0)
            perror("Error in wait() of main process\n");
    }

    return 0;
}
