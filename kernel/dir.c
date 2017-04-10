#include <linux/buffer_head.h>
#include <linux/fs.h>
#include "../mfs.h"

extern inline struct mfs_inode_info *GET_MFS_INODE(struct inode *);
extern struct inode *mfs_iget(struct super_block *, unsigned long);

int mfs_iterate(struct file *file, struct dir_context *dir_context) {
	struct mfs_directory_entry 	*de;
	struct buffer_head 		*bh;
	struct mfs_inode_info		*minode_info;
	struct inode			*inode = file_inode(file);
	int				block_num, i;
	int				size;

	printk(KERN_EMERG "MicroFS:: calling %s pos %lld", __func__, dir_context->pos);
	dump_stack();

	if (dir_context->pos >= MFS_DIR_MAX_ENT) {
		return 0;
	}
	if(dir_context->pos == 0) {
		dir_emit_dots(file, dir_context);
	} else {
		printk(KERN_EMERG "MicroFS:: UNEXPECTED");
		return 0;
	}

	/*
	 * ToDo: check correctness of  dir_context->pos
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

	minode_info = GET_MFS_INODE(inode);
	block_num = minode_info->mi_blk_add[0];
	bh = sb_bread(inode->i_sb, block_num);
	if (!bh) {
		printk(KERN_EMERG "MicroFS:: Error in reading directory");
		return 0;
	}
	de = (struct mfs_directory_entry *)bh->b_data;
	i = dir_context->pos ;

	/* 
	 * ASSUMTION : directory inode will have only one block of data
	 */

	while (i < MFS_DIR_MAX_ENT) { 
		if (le32_to_cpu((de + i)->inode_num)) {
			size = strnlen((de + i)->name, MFS_DIRECTORY_NAME_SIZE);
			if (!dir_emit(dir_context, (de + i)->name, size, le16_to_cpu((de + i)->inode_num), DT_DIR)) {
				brelse(bh);
				return 0;
			}
		}
		i++;
		dir_context->pos = i;
	}
	brelse(bh);
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
	struct mfs_directory_entry 	*de;
	struct buffer_head 		*bh;
	struct mfs_inode_info		*minode_info;
	struct inode			*inode = NULL;
	int				i, j, flag;

	/* 
	 * 1. check dentry->name  is present in directory
	 * 2. read corresponding inode in memory
	 * 3. initialize dentry object with inode found else initialize with NULL
	 */

	printk(KERN_EMERG "MicroFS:: Calling %s :  looking for inode %lu: dir name : %s", __func__, dir->i_ino, dentry->d_name.name);
	dump_stack();
	minode_info = GET_MFS_INODE(dir);
	flag = 0;
	for(i = 0; i < dir->i_blocks; i++) {
		printk(KERN_EMERG "MicroFS:: READING BLOCK %u", minode_info->mi_blk_add[i]);
		bh = sb_bread(dir->i_sb, minode_info->mi_blk_add[i]);
		de = (struct mfs_directory_entry *)bh->b_data;
		if (!bh) {
			printk(KERN_EMERG "MicroFS:: Error in reading directory");
			return NULL;
		}
		for(j = 0; j < MFS_DIR_MAX_ENT; j++) {
			if (le32_to_cpu(de->inode_num) != 0 && strcmp(de->name, dentry->d_name.name) == 0) {
				flag = 1;
				break;
			}
			de++;
		}
		brelse(bh);
	}
	if(flag == 1) {
		inode = mfs_iget(dir->i_sb, le32_to_cpu(de->inode_num));
                if (IS_ERR(inode)) {
                        return ERR_CAST(inode);
                }
	}
	d_add(dentry, inode);
	return NULL;
}

const struct inode_operations mfs_dir_inops = {
	.create			= mfs_create,
	.lookup			= mfs_lookup,
	//.link			= mfs_link,
	//.unlink		= mfs_unlink,
	//.rename		= mfs_rename,
};
