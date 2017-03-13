#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include "../mfs.h"

#define FS_AUTHOR "MicroFS team"
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

/*
 * creating global variable to maintain pointer to in-memory inode slab
 */

struct kmem_cache *mfs_inode_cachep;

/*
 * contructor for slab to initialize linux inode
 */

static void init_once(void *foo)
{
	struct mfs_inode_info *im_minode = foo;

	/*
	 * initialize default fields of inode
	 */

	inode_init_once(&im_minode->vfs_inode);
}

static int __init mfs_register(void) {
	int err;

	/*
	 * create slap for storing in memory inode
	 */
	
	mfs_inode_cachep = kmem_cache_create("mfs_inode_cache", sizeof(struct mfs_inode_info), 0, (SLAB_RECLAIM_ACCOUNT | SLAB_MEM_SPREAD), init_once);
   	if (mfs_inode_cachep == NULL) {
		return -ENOMEM;
	}
	printk(KERN_INFO "Register file system");
	err = register_filesystem(&mfs_file_system_type);
	if (err) {

		/*
		 * wait for flush
		 */

		rcu_barrier();
		kmem_cache_destroy(mfs_inode_cachep);
	}
	return err;
}

static void __exit mfs_unregister(void) {
	rcu_barrier();
	kmem_cache_destroy(mfs_inode_cachep);
	printk(KERN_INFO "Unregister file system");
	unregister_filesystem(&mfs_file_system_type);
}

module_init(mfs_register);
module_exit(mfs_unregister);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(FS_AUTHOR);
MODULE_DESCRIPTION(FS_DESC);
