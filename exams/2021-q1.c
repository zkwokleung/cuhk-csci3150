#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
    int fd;
    int p1_fd[2];
    pipe(p1_fd);
    fd = open("result", O_CREAT | O_WRONLY | O_TRUNC, 00644);
    if (fork() > 0)
    {
        // Area 1 starts
        close(1);
        dup(p1_fd[1]);
        // Area 1 ends
        close(p1_fd[0]);
        close(p1_fd[1]);
        close(fd);
        execl("/bin/ls", "ls", "-l", NULL);
    }
    else
    {
        // Area 2 starts
        close(0);
        dup(p1_fd[0]);
        // Area 2 ends
        close(p1_fd[0]);
        close(p1_fd[1]);
        close(fd);
        execl("/bin/wc", "wc", "-l", NULL);
    }
}
