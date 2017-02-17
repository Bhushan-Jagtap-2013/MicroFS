#include <linux/fs.h>

/* mfs_fill_super is expected to fill super block information from device
 * This code is just place holder for the function. Please fill the function
 * with required steps and remove the comment.
 */

int mfs_fill_super(struct super_block *sb, void *data, int silent) {
	printk(KERN_EMERG "MicroFS:: Mounting file system");
	dump_stack();
	return 0;
}
