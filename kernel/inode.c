#include <linux/fs.h>
#include <linux/buffer_head.h>
//#include <linux/slab.h>
#include "../mfs.h"

/*
 * This is internal implemetaion of iget(). iget() support has been removed
 * since linux 2.2.26. This will get a new inode and fill contents from
 * device.
 *
 * If inode is present in cache it will return it. If inode is not present
 * then it will read from device and return.
 */

struct inode *mfs_iget(struct super_block *sb, unsigned long ino)
{
	struct buffer_head * bh;
	struct inode *inode;

	inode = iget_locked(sb, ino);
	if (!inode)
		return ERR_PTR(-ENOMEM);
	if (!(inode->i_state & I_NEW))
		return inode;
}
