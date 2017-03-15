#include <linux/fs.h>
#include <linux/buffer_head.h>

int mfs_iterate(struct file *file, struct dir_context *dir_context) {
	struct mfs_directory_entry 	*de;
	struct buffer_head 		*bh;
	struct inode			*inode = file_inode(file);

	printk(KERN_EMERG "MicroFS:: Implementation for %s is not provided", __func__);
	dump_stack();
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
