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
int devfd;

int read_inode(int block_no, int offset, struct mfs_inode *uip)
{	
//	printf("imap.map[2] : %d \n",imap.map[2]);

	lseek(devfd,((block_no*MFS_BLOCKSIZE)+offset),SEEK_SET);
	read(devfd, (char *)uip, sizeof(struct mfs_inode));
	return 0;
}

int read_sbnode(int block_no, int offset,struct mfs_super_block *usp)
{
	lseek(devfd,((block_no*MFS_BLOCKSIZE)+offset),SEEK_SET);
	read(devfd, (char *)usp, sizeof(struct mfs_super_block));
	return 0;
}

void print_sbnode(struct mfs_super_block *usp)
{
	printf("msb_magic                :0x%x\n",usp->msb_magic);
        printf("msb_n_free_inode         : %d \n",usp->msb_n_free_inode);
        printf("msb_n_free_blks          : %d \n",usp->msb_n_free_blks);

}

void print_inode(struct mfs_inode *uip)
{
	char buf[MFS_BLOCKSIZE];
	struct mfs_directory_entry *dirent;
	int i,x;


//	printf("\nInode numner : %d \n",inum);
	printf("mi_mode            : %x\n",uip->mi_mode);
	printf("mi_uid             : %d\n",uip->mi_uid);
	printf("mi_gid             : %d\n",uip->mi_gid);
	printf("mi_atime           : %s\n",ctime((time_t *)&uip->mi_atime));
	printf("mi_ctime           : %s\n",ctime((time_t *)&uip->mi_ctime));
	printf("mi_mtime           : %s\n",ctime((time_t *)&uip->mi_mtime));
	printf("mi_links_count     : %d\n",uip->mi_links_count);
	printf("mi_size            : %d\n",uip->mi_size);
	printf("mi_blocks          : %d\n",uip->mi_blocks);
	
	for(i=0;i<MFS_IBLOCK_COUNT;i++)
	{
		if(i%4==0)
			printf("\n");
		printf("mi_blk_add[%2d]:%3d\t",i,uip->mi_blk_add[i]);
	}
	printf("\n");

	/* Print out the directory entries */

	if(uip->mi_mode & S_IFDIR)
	{
		printf("\n\n Directory entries : \n");
		printf("\tInode_number\tDirectory_name\n\n");
		
		for(i=0;i<uip->mi_blocks;i++)
		{
			lseek(devfd,uip->mi_blk_add[i]*MFS_BLOCKSIZE,SEEK_SET);
			read(devfd,buf,MFS_BLOCKSIZE);
			dirent = (struct mfs_directory_entry *)buf;
			for(x=0;x<MFS_IBLOCK_COUNT;x++)
			{
				if(dirent->inode_num!=0)
				{
					printf("%d\t%2d\t\t%s\n",x,dirent->inode_num,dirent->name);
				}
				dirent++;
			}

		}
	printf("\n");	
		
	}
	else
	{
		printf("\n\n");
	}
}


int main(int argc,char **argv)
{
	struct mfs_inode inode;
	int block_no,offset,i;

	if(argc !=5)
	{
		printf("\n mfsdb.mfs <device_name  block_number  off_set  structure_name>");
		exit(1);
	}

	devfd = open(argv[1], O_RDWR);

	if(devfd < 0)
	{
		printf("Failed to open the device \n");
	}

//	read(devfd, (char *)&sb, sizeof(struct mfs_super_block));

/*	if(sb.msb_magic != MFS_MAGIC)
	{
		printf("This is not the mfs filesystem \n");
		exit(1);
	}
*/
	block_no=atoi(argv[2]);
	offset=atoi(argv[3]);

	if(strcmp(argv[4],"inode")==0)
	{
		read_inode(block_no,offset,&inode);
		print_inode(&inode);
	}

	if(strcmp(argv[4],"superblock")==0)
	{
		read_sbnode(block_no,offset,&sb);
		print_sbnode(&sb);
	}
	if(strcmp(argv[4],"inodemap")==0)
	{
		lseek(devfd,((block_no*MFS_BLOCKSIZE)+offset),SEEK_SET);
		read(devfd,(char *)&imap,sizeof(struct mfs_inode_map));
		printf("%d",imap.map[0]);
		for(i=1;i<1024;i++)
		{
			if(i%8==0)
				printf(" ");
			if(i%64==0)
				printf("\n");
			printf("%d",imap.map[i]);
			
		}
		printf("\n");
	}
	if(strcmp(argv[4],"blockmap")==0)
	{
		lseek(devfd,((block_no*MFS_BLOCKSIZE)+offset),SEEK_SET);
                read(devfd,(char *)&bmap,sizeof(struct mfs_block_map));
                printf("%d",bmap.map[0]);
                for(i=1;i<1024;i++)
                {
                        if(i%8==0)
                                printf(" ");
                        if(i%64==0)
                                printf("\n");
                        printf("%d",bmap.map[i]);

                }
		printf("\n");
	}
	
		
	return 0;
}
