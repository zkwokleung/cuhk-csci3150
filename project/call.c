#include "call.h"
const char *HD = "HD";

int open_t(char *pathname)
{
    int inode_number = 0;
    // Current working directory
    char *wd = malloc(strlen(pathname) + 1);
    char *token;
    inode *ip = malloc(sizeof(inode));
    DIR_NODE *dir_node = (DIR_NODE *)malloc(BLK_SIZE);

    int currpos;

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
        currpos = lseek(fd, I_OFFSET + inode_number * sizeof(inode), SEEK_SET);
        if (currpos < 0)
        {
            printf("!!! Error ruuning lseek() while reading inode\n");
            return -1;
        }

        // Read the inode from disk
        if (read(fd, ip, sizeof(inode)) != sizeof(inode))
        {
            printf("!!! Error running read() while reading inode\n");
            return -1;
        }

        if (ip->f_type == DIR)
        {
            // Read the directory mappings
            currpos = lseek(fd, D_OFFSET + ip->direct_blk[0] * BLK_SIZE, SEEK_SET);
            if (currpos < 0)
            {
                printf("!!! Error ruuning lseek() while reading dir_node\n");
                return -1;
            }

            // Read the directory mappings from disk
            if (read(fd, dir_node, BLK_SIZE) != BLK_SIZE)
            {
                printf("!!! Error running read() while reading dir_node\n");
                return -1;
            }

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
    free(ip);
    free(dir_node);

    return inode_number;
}

int read_t(int i_number, int offset, void *buf, int count)
{
    int read_bytes;

    // Open the HD file
    int fd = open(HD, O_RDWR);
    if (fd < 0)
    {
        printf("Error: open()\n");
        return -1;
    }

    // Read the inode
    inode *ip = read_inode(fd, i_number);
    if (ip == NULL)
    {
        printf("Error: read_inode()\n");
        return -1;
    }

    // Read the data

    return read_bytes;
}

// you are allowed to create any auxiliary functions that can help your implementation. But only "open_t()" and
// "read_t()" are allowed to call these auxiliary functions.