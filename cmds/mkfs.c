#include <linux/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "../mfs.h"

int main(int argc, char **argv)
{
	struct mfs_super_block		sb;
	struct mfs_inode_map		imap;
	struct mfs_block_map		bmap;
	struct mfs_inode		inode;
	struct mfs_directory_entry	dir;
	time_t				ctime = time(NULL);
	char				block[1024];
	int				fd;

	if (argc != 2) {
		printf("mkfs.mfs <device>\n");
		exit(1);
	}
	fd = open(argv[1], O_WRONLY);
	if (fd < 0) {
		printf("Failed to open device\n");
		exit(1);
	}

	/*
	 * Write super block on device.
	 */

	sb.msb_magic = MFS_MAGIC;

	/*
	 * inode 0 and 1 - not used
	 * inode 2 	 - used for root directory
	 * inode 3 	 - used for lost+found
	 */

	sb.msb_n_free_inode = MFS_MAX_NUM_INODE - 4;

	/*
	 * block 0 - used for root inode
	 * block 1 - used for lost+found
	 */

	sb.msb_n_free_blks = MFS_MAX_NUM_BLK - 2;
	write(fd, (char *)&sb, sizeof(struct mfs_super_block));

	/*
	 * Initialize inode map.Initial 4 inodes are in use.
	 */

	memset((void *)&imap, 0, sizeof(struct mfs_inode_map));
	imap.map[0] = USED;
	imap.map[1] = USED;
	imap.map[2] = USED;
	imap.map[3] = USED;
	lseek(fd, 1 * MFS_BLOCKSIZE, SEEK_SET);
	write(fd, (char *)&imap, sizeof(struct mfs_inode_map));

	/*
	 * Initialize block map.Initial 2 blocks are in use.
	 */

	memset((void *)&bmap, 0, sizeof(struct mfs_block_map));
	bmap.map[0] = USED;
	bmap.map[1] = USED;
	lseek(fd, 2 * MFS_BLOCKSIZE, SEEK_SET);
	write(fd, (char *)&bmap, sizeof(struct mfs_block_map));

	/*
	 * Initialize inode 0 and 1 with zeros.
	 */

	memset((void *)&inode, 0, sizeof(struct mfs_inode));
	lseek(fd, MFS_ILIST_START_BLOCK_NUM * MFS_BLOCKSIZE, SEEK_SET);
	write(fd, (char *)&inode, sizeof(struct mfs_inode));
	write(fd, (char *)&inode, sizeof(struct mfs_inode));

	/*
	 * Initialize root inode.
	 */

	memset((void *)&inode, 0, sizeof(struct mfs_inode));
	inode.mi_mode = S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO;
	inode.mi_uid = 0;
	inode.mi_gid = 0;
	inode.mi_atime = ctime;
	inode.mi_ctime = ctime;
	inode.mi_mtime = ctime;
	inode.mi_links_count = 3;	/* directory ".", ".." and lost+found are referring root directory */
	inode.mi_size = MFS_BLOCKSIZE;
	inode.mi_blocks = 1;
	inode.mi_blk_add[0] = MFS_BLIST_START_BLOCK_NUM;
	write(fd, (char *)&inode, sizeof(struct mfs_inode));

	/* 
	 * Initialize lost+found inode.
	 */

	memset((void *)&inode, 0, sizeof(struct mfs_inode));
	inode.mi_mode = S_IFDIR | S_IRWXU | S_IRWXG | S_IRWXO;
	inode.mi_uid = 0;
	inode.mi_gid = 0;
	inode.mi_atime = ctime;
	inode.mi_ctime = ctime;
	inode.mi_mtime = ctime;
	inode.mi_links_count = 2;	/* directory ".", ".." are referring lost+found directory */
	inode.mi_size = MFS_BLOCKSIZE;
	inode.mi_blocks = 1;
	inode.mi_blk_add[0] = MFS_BLIST_START_BLOCK_NUM + 1;
	write(fd, (char *)&inode, sizeof(struct mfs_inode));

	
	/*
	 * Zero-out first 2 blocks, as we want to use them.
	 */

	lseek(fd, MFS_BLIST_START_BLOCK_NUM * MFS_BLOCKSIZE, SEEK_SET);
	memset((void *)&block, 0, sizeof(block));
	write(fd, &block, sizeof(block));
	write(fd, &block, sizeof(block));

	/*
	 * Create directory entry for root inode.
	 */

	lseek(fd, MFS_BLIST_START_BLOCK_NUM * MFS_BLOCKSIZE, SEEK_SET);
	dir.inode_num = 2;
	strcpy(dir.name, ".");
	write(fd, &dir, sizeof(struct mfs_directory_entry));

	dir.inode_num = 2;
	strcpy(dir.name, "..");
	write(fd, &dir, sizeof(struct mfs_directory_entry));

	dir.inode_num = 3;
	strcpy(dir.name, "lost+found");
	write(fd, &dir, sizeof(struct mfs_directory_entry));

	/*
	 * Create directory entry for lost+found.
	 */

	lseek(fd, (MFS_BLIST_START_BLOCK_NUM + 1) * MFS_BLOCKSIZE, SEEK_SET);
	dir.inode_num = 3;
	strcpy(dir.name, ".");
	write(fd, &dir, sizeof(struct mfs_directory_entry));

	dir.inode_num = 2;
	strcpy(dir.name, "..");
	write(fd, &dir, sizeof(struct mfs_directory_entry));
	
	close(fd);
	return 0;
}
