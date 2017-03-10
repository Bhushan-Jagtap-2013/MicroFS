#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

#define FS_AUTHOR "Bhushan Jagtap"
#define FS_DESC "Simple file system module"


/*
 * Register a file system with linux.
 */

extern int mfs_fill_super(struct super_block *sb, void *data, int silent);

static struct dentry *mfs_mount(struct file_system_type *fs_type, int flags, const char *dev_name, void *data) {
        return mount_bdev(fs_type, flags, dev_name, data, mfs_fill_super);
}

static struct file_system_type mfs_file_system_type = {
	.owner		= THIS_MODULE,
	.name		= "mfs",
	.mount		= mfs_mount,
	.kill_sb        = kill_block_super,
	.fs_flags       = FS_REQUIRES_DEV,
};

static int __init mfs_register(void) {
	int err;

	printk(KERN_INFO "Register file system");
	err = register_filesystem(&mfs_file_system_type);
	return err;
}

static void __exit mfs_unregister(void) {
	printk(KERN_INFO "Unregister file system");
	unregister_filesystem(&mfs_file_system_type);
}

module_init(mfs_register);
module_exit(mfs_unregister);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(FS_AUTHOR);
MODULE_DESCRIPTION(FS_DESC);
