#define MFS_BLOCKSIZE		1024
#define MFS_MAGIC		0x12345678
#define MFS_MAX_NUM_INODE	1024
#define MFS_MAX_NUM_BLK		1024

/*
 * Block Number		Usage
 * 0			super block
 * 1			inode usage map
 * 2			block usage map
 * 3 - 66		start of inode structure array (ilist)
 * 			# of block used = MFS_MAX_NUM_INODE / 16 (# of inode per block) = 64 blocks
 * 67			start of blocks
 */

#define MFS_ILIST_START_BLOCK_NUM	3
#define MFS_BLIST_START_BLOCK_NUM	67

struct mfs_super_block {
	__u32	msb_magic;
	__u32	msb_n_free_inode;
	__u32	msb_n_free_blks;
};

struct mfs_super_block_info {
	struct buffer_head *sbi_sb_bh; 
	struct mfs_super_block *sbi_msb;
};

struct mfs_inode_map {
	__u8	map[MFS_MAX_NUM_INODE];
};

struct mfs_block_map {
	__u8	map[MFS_MAX_NUM_BLK];
};

/* 
 * MFS_IBLOCK_COUNT will make structure aligned with 64 byte boundry.
 */

#define MFS_IBLOCK_COUNT	7
#define MFS_ROOT_INODE		2
#define MFS_MAX_INODE		1024
#define	MFS_INODE_PER_BLOCK	16

#define USED			1
#define UNUSED			0

struct mfs_inode {
	__u32	mi_mode;			/* File mode */
	__s32	mi_uid;				/* Owner Uid */
	__s32	mi_gid;				/* Group Id */
	__u32	mi_atime;			/* Access time */
	__u32	mi_ctime;			/* Creation time */
	__u32	mi_mtime;			/* Modification time */
	__u32	mi_links_count;			/* Links count */
	__u32	mi_size;			/* Size in bytes */
        __u32	mi_blocks;			/* Block's count */
        __u32	mi_blk_add[MFS_IBLOCK_COUNT];	/* Block address */
};

#ifdef __KERNEL__

struct mfs_inode_info {
        __u32	mi_blk_add[MFS_IBLOCK_COUNT];	/* Block address */
	struct inode vfs_inode;
};

#endif

#define MFS_DIRECTORY_NAME_SIZE		12
#define MFS_DIR_MAX_ENT			64

struct mfs_directory_entry {
	__le32	inode_num;
	char	name[MFS_DIRECTORY_NAME_SIZE];
};
