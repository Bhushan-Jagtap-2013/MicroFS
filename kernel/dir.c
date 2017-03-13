#include <linux/fs.h>

int mfs_iterate(struct file *file, struct dir_context *dir_context) {
	dump_stack();
	return 0;
}

const struct file_operations mfs_dir_ops = {
	.read		= generic_read_dir,
	.iterate	= mfs_iterate,
	.fsync		= generic_file_fsync,
	.llseek		= generic_file_llseek,
};

const struct inode_operations mfs_dir_inops = {
	/*
	.create			= mfs_create,
	.lookup			= mfs_lookup,
	.link			= mfs_link,
	.unlink			= mfs_unlink,
	.rename			= mfs_rename,
	*/
};
