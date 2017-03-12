#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
#include "../mfs.h"

/* 
 * Superblock operations
 */

extern struct inode *mfs_iget(struct super_block *, unsigned long);

static const struct super_operations mfs_sops = {
/*
	.alloc_inode	= bfs_alloc_inode,
	.destroy_inode	= bfs_destroy_inode,
	.write_inode	= bfs_write_inode,
	.evict_inode	= bfs_evict_inode,
	.put_super	= bfs_put_super,
	.statfs		= bfs_statfs,
*/
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

	/*
	 * ToDo: Replace code that manually initialize super block from device.
	 */

	msb->msb_magic = MFS_MAGIC; 		/* Remove : read from device */
	if(msb->msb_magic != MFS_MAGIC) {
		printk(KERN_EMERG "MicroFS : Magic Number mis-match");
		goto release_bh;
	}
	msb->msb_n_free_inode = 1024;		/* Remove : read from device */
	msb->msb_n_free_blks = 1024;		/* Remove : read from device */

	/*
	 * Initialize incore super block structure of mfs.
	 */

	msbi = (struct mfs_super_block_info *) kzalloc(sizeof(struct mfs_super_block_info), GFP_KERNEL);
	msbi->sbi_msb = msb;
	msbi->sbi_sb_bh = bh;

	/* 
	 * Initialize linux super block.
	 */

	sb->s_magic = MFS_MAGIC;	
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
