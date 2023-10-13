#ifndef _INODE_H_
#define _INODE_H_
#include <time.h>

#define FILE 0
#define DIR 1

typedef struct _inode_ 
{
	int i_number;
	int c_time;	// use int, detailed reason can be found at lab-04-Prog/inode.c and lab-04-Prog/inode.h
	int f_type;
	int f_size;
	int blk_number;
	int direct_blk[2];
	int indirect_blk;
	int sub_f_num;
}inode;

typedef struct dir_mapping
{
	char f_name[20];
	int i_number;
}DIR_NODE;

inode* read_inode(int fd, int inode_number);
void print_inode_info(inode* ip);
void print_inode_region(int fd, int inode_number);
void print_dir_mappings(int fd, int inode_number);
#endif
