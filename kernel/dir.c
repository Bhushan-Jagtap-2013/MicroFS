#include <linux/fs.h>
#include <linux/buffer_head.h>
#include "../mfs.h"

extern inline struct mfs_inode_info *GET_MFS_INODE(struct inode *);

int mfs_iterate(struct file *file, struct dir_context *dir_context) {
	struct mfs_directory_entry 	*de;
	struct buffer_head 		*bh;
	struct mfs_inode_info		*minode_info;
	struct inode			*inode = file_inode(file);
	int				block_num, index, i;

	printk(KERN_EMERG "MicroFS:: Implementation for %s is not provided", __func__);
	dump_stack();

	/*
	 * ToDo: check correctnes dir_context->pos
	 */

	/*
	 * Steps we to follow :
	 * 1. linux will provide us how much we have read earlier in dir_context->pos
	 * 2. from dir_context->pos find the correct block number and offset within block to read contents
	 * 3. read corresponding block from device
	 * 4. keep filling directory entries in dir_context until
	 * 	o we reach at end of the file
	 * 	o dir_emit() failed to fill entries
	 * 5. if we will encounter at end of the block. read next from number from inode then adjust offset and start reading again
	 */

	while (dir_context->pos < inode->i_size) {
		index = dir_context->pos / MFS_BLOCKSIZE;
		minode_info = GET_MFS_INODE(inode);
		block_num = minode_info->mi_blk_add[index];
		bh = sb_bread(inode->i_sb, block_num);
		if (!bh) {
			printk(KERN_EMERG "MicroFS:: Error in reading directory");
			return 0;
		}
		de = (struct mfs_directory_entry *)bh->b_data;
		i = (dir_context->pos % MFS_BLOCKSIZE) / sizeof(struct mfs_directory_entry);
		while (i < MFS_DIR_MAX_ENT) { 
			printk(KERN_EMERG "MicroFS:: direcory offset : %u block number : %u inode num %u", i, block_num, de->inode_num);
			if (de->inode_num) {
				if (!dir_emit(dir_context, de->name, sizeof(struct mfs_directory_entry), de->inode_num, DT_UNKNOWN)) {
					brelse(bh);
					return 0;
				}
			}
			i++;
			dir_context->pos += sizeof(struct mfs_directory_entry); 
		}
		brelse(bh);
	}
	return 0;
}

const struct file_operations mfs_dir_ops = {
	.read		= generic_read_dir,
	.iterate	= mfs_iterate,
	.fsync		= generic_file_fsync,
	.llseek		= generic_file_llseek,
};

static int mfs_create(struct inode *dir, struct dentry *dentry, umode_t mode, bool excl) {
	printk(KERN_EMERG "MicroFS:: Implementation for %s is not provided", __func__);
	dump_stack();
	return 0;
}

static struct dentry *mfs_lookup(struct inode *dir, struct dentry *dentry, unsigned int flags) {
	printk(KERN_EMERG "MicroFS:: Implementation for %s is not provided", __func__);
	dump_stack();
	return NULL;
}

const struct inode_operations mfs_dir_inops = {
	.create			= mfs_create,
	.lookup			= mfs_lookup,
	//.link			= mfs_link,
	//.unlink		= mfs_unlink,
	//.rename		= mfs_rename,
};
