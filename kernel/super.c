#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
#include "../mfs.h"

/* 
 * Superblock operations
 */

extern struct inode *mfs_iget(struct super_block *, unsigned long);
extern struct inode *mfs_alloc_inode(struct super_block *sb);
extern void mfs_destroy_inode(struct inode *);
extern int mfs_write_inode(struct inode *, struct writeback_control *wbc);
extern int mfs_drop_inode(struct inode *);
extern void mfs_evict_inode(struct inode *);

void mfs_put_super(struct super_block *sb) {

	/* 
	 * ToDo : Provide implementation
	 */

	printk(KERN_EMERG "MicroFS:: Implementation for %s is not provided", __func__);
	dump_stack();
}

int mfs_statfs(struct dentry *dentry, struct kstatfs *buff) {

	/* 
	 * ToDo : Provide implementation
	 */

	printk(KERN_EMERG "MicroFS:: Implementation for %s is not provided", __func__);
	dump_stack();
	return 0;
}

static const struct super_operations mfs_sops = {
	.alloc_inode	= mfs_alloc_inode,
	.destroy_inode	= mfs_destroy_inode,
	.write_inode	= mfs_write_inode,
	.drop_inode	= mfs_drop_inode,
	.evict_inode	= mfs_evict_inode,
	.put_super	= mfs_put_super,
	.statfs		= mfs_statfs,
};

/* mfs_fill_super is expected to fill super block information from device
 * This code is just place holder for the function. Please fill the function
 * with required steps and remove the comment.
 */

int mfs_fill_super(struct super_block *sb, void *data, int silent) {
	struct mfs_super_block_info	*msbi;
	struct mfs_super_block		*msb;
	struct buffer_head		*bh;
	struct inode			*root;
	int				ret = -EINVAL;

	printk(KERN_EMERG "MicroFS:: Mounting file system");
	dump_stack();


	if (!(ret = sb_set_blocksize(sb, MFS_BLOCKSIZE))) {
		printk(KERN_EMERG "MicroFS : Failed to set block size to %d : Error %d", MFS_BLOCKSIZE, ret);
		goto failed;
	}

	if (!(bh = sb_bread(sb, 0))) {
		printk(KERN_EMERG "MicroFS : Unable to read super block");
                goto failed;
	}
	msb = (struct mfs_super_block *)bh->b_data;
	if(msb->msb_magic != MFS_MAGIC) {
		printk(KERN_EMERG "MicroFS : Magic Number mis-match");
		goto release_bh;
	}

	printk(KERN_EMERG "MicroFS : %x %u %u", msb->msb_magic, msb->msb_n_free_inode, msb->msb_n_free_blks);

	/*
	 * Initialize incore super block structure of mfs.
	 */

	msbi = (struct mfs_super_block_info *) kzalloc(sizeof(struct mfs_super_block_info), GFP_KERNEL);
	msbi->sbi_msb = msb;
	msbi->sbi_sb_bh = bh;

	/* 
	 * Initialize linux super block.
	 */

	sb->s_magic = msb->msb_magic;	
	sb->s_fs_info = msbi;
	sb->s_op = &mfs_sops;

	/*
	 * Read root inode from device and create dentry for it.
	 */

	root = mfs_iget(sb, MFS_ROOT_INODE);
	sb->s_root = d_make_root(root);
        if (!sb->s_root) {
		printk(KERN_EMERG "MicroFS : get root inode failed");
                ret = -ENOMEM;
                goto release_msbi;
        }
	printk(KERN_EMERG "MicroFS : HERE 1");
	return 0;

release_msbi:
	printk(KERN_EMERG "MicroFS : HERE 2");
	kfree(msbi);

release_bh:

	/*
	 * ToDo: release bh when file system is unmounted.
	 */

	printk(KERN_EMERG "MicroFS : HERE 3");
	brelse(bh);

failed:
	printk(KERN_EMERG "MicroFS : HERE 4");
	return ret;
}
