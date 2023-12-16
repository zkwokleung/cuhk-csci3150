#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
int main()
{
    int fd;
    int p1_fd[2], p2_fd[2], p3_fd[2], p4_fd[2];
    pipe(p1_fd);
    pipe(p2_fd);
    pipe(p3_fd);
    pipe(p4_fd);
    fd = open("result", O_CREAT | O_WRONLY | O_TRUNC, 00644);
    if (fork() > 0)
    {
        // Area 1 starts
        close(1);
        dup(p1_fd[1]);
        // Area 1 ends
        close(p1_fd[0]);
        close(p1_fd[1]);
        close(p2_fd[0]);
        close(p2_fd[1]);
        close(p3_fd[0]);
        close(p3_fd[1]);
        close(p4_fd[0]);
        close(p4_fd[1]);
        close(fd);
        execl("/bin/ls", "ls", "-l", NULL);
    }
    else
    {
        if (fork() > 0)
        {
            /* The pipe between "ls" and "sort" */
            // Area 2 starts
            close(0);
            dup(p1_fd[0]);
            // Area 2 ends

            /* The pipe between "sort" and "head" */
            // Area 3 starts
            close(1);
            dup(p2_fd[1]);
            // Area 3 ends
            close(p1_fd[0]);
            close(p1_fd[1]);
            close(p2_fd[0]);
            close(p2_fd[1]);
            close(p3_fd[0]);
            close(p3_fd[1]);
            close(p4_fd[0]);
            close(p4_fd[1]);
            close(fd);
            execl("/bin/sort", "sort", NULL);
        }
        else
        {
            if (fork() > 0)
            {
                /* The pipe between "sort" and "head" */
                // Area 4 starts
                close(0);
                dup(p2_fd[0]);
                // Area 4 ends

                /* The pipe between "head" and "grep" */
                // Area 5 starts
                close(1);
                dup(p3_fd[1]);
                // Area 5 ends

                close(p1_fd[0]);
                close(p1_fd[1]);
                close(p2_fd[0]);
                close(p2_fd[1]);
                close(p3_fd[0]);
                close(p3_fd[1]);
                close(p4_fd[0]);
                close(p4_fd[1]);
                close(fd);
                execl("/bin/head", "head", "-n", "3", NULL);
            }
            else
            {
                if (fork() > 0)
                {
                    /* The pipe between "head" and "grep" */
                    // Area 6 starts
                    close(0);
                    dup(p3_fd[0]);
                    // Area 6 ends

                    /* The pipe between "grep" and "wc" */
                    // Area 7 starts
                    close(1);
                    dup(p4_fd[1]);
                    // Area 7 ends
                    close(p1_fd[0]);
                    close(p1_fd[1]);
                    close(p2_fd[0]);
                    close(p2_fd[1]);
                    close(p3_fd[0]);
                    close(p3_fd[1]);
                    close(p4_fd[0]);
                    close(p4_fd[1]);
                    close(fd);
                    execl("/bin/grep", "grep", "midterm", NULL);
                }
                else
                {
                    /* The pipe between "grep" and "wc" */
                    // Area 8 starts
                    close(0);
                    dup(p4_fd[0]);
                    // Area 8 ends
                    close(p1_fd[0]);
                    close(p1_fd[1]);
                    close(p2_fd[0]);
                    close(p2_fd[1]);
                    close(p3_fd[0]);
                    close(p3_fd[1]);
                    close(p4_fd[0]);
                    close(p4_fd[1]);
                    close(fd);
                    execl("/bin/wc", "wc", "-l", NULL);
                }
            }
        }
    }
}