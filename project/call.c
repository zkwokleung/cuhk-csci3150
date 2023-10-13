#include "call.h"
const char *HD = "HD";

int open_t(char *pathname)
{
    int inode_number = 0;
    char *wd = malloc(strlen(pathname) + 1);
    char *token;
    inode *ip;
    DIR_NODE *dir_node = (DIR_NODE *)malloc(BLK_SIZE);

    // Open the HD file
    int fd = open(HD, O_RDWR);
    if (fd < 0)
    {
        printf("Error: open()\n");
        return -1;
    }

    // Seperate the path by '/'
    strcpy(wd, pathname);
    token = strtok(wd, "/");
    while (token != NULL)
    {
        // Read the inode
        ip = read_inode(fd, inode_number);
        if (ip == NULL)
        {
            printf("Error: read_inode()\n");
            return -1;
        }

        if (ip->f_type == DIR)
        {
            // Read the directory mappings
            int currpos = lseek(fd, D_OFFSET + ip->direct_blk[0] * BLK_SIZE, SEEK_SET);
            read(fd, dir_node, BLK_SIZE);

            for (int i = 0; i < ip->sub_f_num; i++)
            {
                // Check if the token is the same as the directory name
                if (strcmp(dir_node[i].f_name, token) == 0)
                {
                    inode_number = dir_node[i].i_number;
                    break;
                }
            }
        }

        // Move to the next token
        token = strtok(NULL, "/");
    }

    // Release
    close(fd);
    free(wd);

    return inode_number;
}

int read_t(int i_number, int offset, void *buf, int count)
{
    int read_bytes;
    // write your code here.
    return read_bytes;
}

// you are allowed to create any auxiliary functions that can help your implementation. But only "open_t()" and
// "read_t()" are allowed to call these auxiliary functions.