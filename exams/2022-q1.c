#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
    int fd;
    int p1_fd[2];
    int pid;

    pipe(p1_fd);

    if ((fd = open("myfile1", O_CREAT | O_WRONLY | O_TRUNC, 00644)) < 0)
    {
        printf("Error: Cannot open file in open()\n");
        exit(-1);
    }

    char *argument_list1[] = {"cat", "mytest", NULL};
    char *argument_list2[] = {"wc", NULL};

    if ((pid = fork()) > 0)
    {
        /* BOX1 BEGIN */
        close(1);
        dup(p1_fd[1]);
        /* BOX1 END */

        close(p1_fd[0]);
        close(p1_fd[1]);
        close(fd);
        execvp("cat", argument_list1);
    }
    else if (pid == 0)
    {
        /* BOX2 BEGIN */
        close(0);
        dup(p1_fd[0]);
        /* BOX2 END */

        close(p1_fd[0]);
        close(p1_fd[1]);
        close(fd);
        execvp("wc", argument_list2);
    }
    else
    {
        return -1;
    }

    return 0;
}
