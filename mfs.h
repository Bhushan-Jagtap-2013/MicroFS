#define MFS_BLOCKSIZE 1024
#define MFS_MAGIC 0x12345678

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
	__u8	map[MFS_BLOCKSIZE];
};

struct mfs_block_map {
	__u8	map[MFS_BLOCKSIZE];
};

struct mfs_inode {
	__u32	mi_mode;				/* File mode */
	__s32	mi_uid;				/* Owner Uid */
	__s32	mi_gid;				/* Group Id */
	__u32	mi_atime;			/* Access time */
	__u32	mi_ctime;			/* Creation time */
	__u32	mi_mtime;			/* Modification time */
	__u32	mi_links_count;			/* Links count */
	__u32	mi_size;				/* Size in bytes */
        __u32	mi_blocks;			/* Blocks count */
        __u32	mi_blk_add[MFS_IBLOCK_COUNT];	/* Blocks count */
}

/* 
 * MFS_IBLOCK_COUNT will make structure aligned with 64 byte boundry.
 */

#define MFS_IBLOCK_COUNT	7
#define MFS_ROOT_INODE		2
#define MFS_MAX_INODE		1024
