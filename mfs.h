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

struct inode_map {
	__u8	map[MFS_BLOCKSIZE];
};

struct block_map {
	__u8	map[MFS_BLOCKSIZE];
};
