#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <time.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <stdlib.h>
#include <string.h>
#include "../mfs.h"


struct mfs_super_block sb;
struct mfs_inode_map imap;
struct mfs_block_map bmap;
char buf[MFS_BLOCKSIZE];
int devfd;

/*
 *  Read the inode block
 */
int read_inode(int block_no, int offset, struct mfs_inode *uip)
{	
	lseek(devfd,((block_no*MFS_BLOCKSIZE)+offset),SEEK_SET);
	read(devfd, (char *)uip, sizeof(struct mfs_inode));
	return 0;
}

/*
 * Read the SuperBlock
 */
int read_sbnode(int block_no, int offset,struct mfs_super_block *usp)
{
	lseek(devfd,((block_no*MFS_BLOCKSIZE)+offset),SEEK_SET);
	read(devfd, (char *)usp, sizeof(struct mfs_super_block));
	return 0;
}

/*
 * Print the SuperBlock details
 */
void print_sbnode(struct mfs_super_block *usp)
{
	printf("msb_magic\t\t:0x%x\n",usp->msb_magic);
        printf("msb_n_free_inode\t: %d \n",usp->msb_n_free_inode);
        printf("msb_n_free_blks\t\t: %d \n",usp->msb_n_free_blks);
}

/*
 * Print the inode details 
 */
void print_inode(struct mfs_inode *uip)
{
	int i;

	printf("mi_mode\t\t: %x\n",uip->mi_mode);
	printf("mi_uid\t\t: %d\n",uip->mi_uid);
	printf("mi_gid\t\t: %d\n",uip->mi_gid);
	printf("mi_atime\t: %s\n",ctime((time_t *)&uip->mi_atime));
	printf("mi_ctime\t: %s\n",ctime((time_t *)&uip->mi_ctime));
	printf("mi_mtime\t: %s",ctime((time_t *)&uip->mi_mtime));
	printf("mi_links_count\t: %d\n",uip->mi_links_count);
	printf("mi_size\t\t: %d\n",uip->mi_size);
	printf("mi_blocks\t: %d\n",uip->mi_blocks);
	
	printf("Block Number\t:");
	for (i=0;i<MFS_IBLOCK_COUNT;i++) {
		printf("%2d\t",i);
	}

	printf("\n");
	printf("Adress\t\t:");
	for (i=0;i<MFS_IBLOCK_COUNT;i++) {
		printf("%3d\t",uip->mi_blk_add[i]);
	}
	printf("\n");
}

/*
 *  Read the directory entry
*/
int read_dir(int block_no,int offset,char *buf)
{
	lseek(devfd,((block_no*MFS_BLOCKSIZE)+offset),SEEK_SET);
	read(devfd,buf,MFS_BLOCKSIZE);
	return 0;
}

/*
 * Print out the directory entry
 */
void print_dir(char *buf)
{
	int x;
	struct mfs_directory_entry *dirent;

	dirent=(struct mfs_directory_entry *)buf;

	printf("\n\nDirectory entries : \n\n");
        printf("Sr.No\tInode_number\tDirectory_name\n\n");

	for (x=0;x<MFS_IBLOCK_COUNT;x++) {
        	if (dirent->inode_num!=0) {
            		printf("%d\t%2d\t\t%s\n",x,dirent->inode_num,dirent->name);
          	}
          	dirent++;
        }
}

/*
 * To disply the usage of the fsdb command
 */
void usage()
{
	printf("\nUsage:\n");
	printf("\n ./fsdb.mfs <device_name>  <block_number>  <off_set>  <structure_name>\n");
	printf("\nDebugging Utility for Micro file system.\n");

	printf("\nOptions :\n");
	printf("<device_name>\t\tpath to the device to be used.\n");
	printf("<block_number>\t\tBlock number of the structure.\n");
	printf("<offset>\t\tOffset value in the block.\n");
	printf("<structure_name>\tStructure name whose details needs to be printed.\n");
	printf("\t\t\t1.superblock 2.inode 3.directory 4.inodemap 5.blockmap\n");
        exit(1);

}

int main(int argc,char **argv)
{
	struct mfs_inode inode;
	int block_no,offset,i;

	if (argc !=5) {
		usage();
	}

	devfd = open(argv[1], O_RDWR);

	if (devfd < 0) {
		printf("Failed to open the device \n");
		exit(1);
	}

	block_no=atoi(argv[2]);
	offset=atoi(argv[3]);

	if (strcmp(argv[4],"inode")==0) {
		read_inode(block_no,offset,&inode);
		print_inode(&inode);
	}

	else if (strcmp(argv[4],"superblock")==0) {
		read_sbnode(block_no,offset,&sb);
		print_sbnode(&sb);
	}

	else if (strcmp(argv[4],"inodemap")==0) {
		lseek(devfd,((block_no*MFS_BLOCKSIZE)+offset),SEEK_SET);
		read(devfd,(char *)&imap,sizeof(struct mfs_inode_map));
		printf("%d",imap.map[0]);
		for (i=1;i<1024;i++) {
			if (i%8==0)
				printf(" ");
			if (i%64==0)
				printf("\n");
			printf("%d",imap.map[i]);
			
		}
		printf("\n");
	}

	else if (strcmp(argv[4],"blockmap")==0) {
		lseek(devfd,((block_no*MFS_BLOCKSIZE)+offset),SEEK_SET);
                read(devfd,(char *)&bmap,sizeof(struct mfs_block_map));
                printf("%d",bmap.map[0]);
                for (i=1;i<1024;i++) {
                        if(i%8==0)
                                printf(" ");
                        if(i%64==0)
                                printf("\n");
                        printf("%d",bmap.map[i]);

                }
		printf("\n");
	}

	else if (strcmp(argv[4],"directory")==0) {
		read_dir(block_no,offset,buf);
		print_dir(buf);
	}
	else
	{
		printf("\nInvalid Command\n");
		usage();
	}

	return 0;
}
