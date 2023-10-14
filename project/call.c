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
    int read_bytes = 0;
    int byte_to_read = 0;
    inode *ip;
    int byte_to_read_remain;
    int byte_in_file_remain;
    int current_block_index = 0;
    int current_block;
    int current_offset;
    int total_offset;

    // Check if the count is valid
    if (count > MAX_FILE_SIZE)
    {
        printf("!!! Error: count is larger than MAX_FILE_SIZE\n");
        return 0;
    }

    // Check if the offset is valid
    if (offset > MAX_FILE_SIZE)
    {
        printf("!!! Error: offset is larger than MAX_FILE_SIZE\n");
        return 0;
    }

    // Open the HD file
    int fd = open(HD, O_RDWR);
    if (fd < 0)
    {
        printf("Error openning HD\n");
        return -1;
    }

    // Read the inode from disk
    if (lseek(fd, I_OFFSET + i_number * sizeof(inode), SEEK_SET) < 0)
    {
        printf("!!! Error ruuning lseek() while reading inode\n");
        return -1;
    }

    ip = malloc(sizeof(inode));
    if (read(fd, ip, sizeof(inode)) != sizeof(inode))
    {
        printf("!!! Error running read() while reading inode\n");
        return -1;
    }

    byte_in_file_remain = ip->f_size - offset;
    byte_to_read_remain = count;
    current_offset = offset;

    current_block = ip->direct_blk[0];

    while (byte_to_read_remain > 0 && current_block != -1 && byte_in_file_remain > 0)
    {
        if (current_offset < BLK_SIZE)
        {
            // ! The offset is within a block

            // Calculate the number of byte to read in the current block
            byte_to_read = byte_to_read_remain;
            if (byte_to_read > BLK_SIZE - current_offset)
            {
                byte_to_read = BLK_SIZE - current_offset;
            }

            // Calculate the number of byte to read in the file
            if (byte_to_read > byte_in_file_remain)
            {
                byte_to_read = byte_in_file_remain;
            }

            byte_in_file_remain -= byte_to_read;
            byte_to_read_remain -= byte_to_read;

            // Read the block
            if (lseek(fd, D_OFFSET + current_block * BLK_SIZE + current_offset, SEEK_SET) < 0)
            {
                printf("!!! Error ruuning lseek() while reading block\n");
                return -1;
            }

            if (read(fd, buf + read_bytes, byte_to_read) != byte_to_read)
            {
                printf("!!! Error running read() while reading block\n");
                return -1;
            }
            read_bytes += byte_to_read;

            total_offset += byte_to_read;
            current_offset = BLK_SIZE;
        }
        else
        {
            // ! The offset is not within a block

            // Move to the next block
            if (++current_block_index < 2)
            {
                current_block = ip->direct_blk[current_block_index];
            }
            else
            {
                int indirect_blk_buf;
                // Read the indirect block
                if (lseek(fd, D_OFFSET + ip->indirect_blk * BLK_SIZE + (current_block_index - 2) * sizeof(int),
                          SEEK_SET) < 0)
                {
                    printf("!!! Error ruuning lseek() while reading indirect block\n");
                    return -1;
                }

                if (read(fd, &current_block, sizeof(int)) != sizeof(int))
                {
                    printf("!!! Error running read() while reading indirect block\n");
                    return -1;
                }
            }
            current_offset -= BLK_SIZE;
        }
    }

    // Release
    close(fd);
    free(ip);

    return read_bytes;
}

// you are allowed to create any auxiliary functions that can help your implementation. But only "open_t()" and
// "read_t()" are allowed to call these auxiliary functions.