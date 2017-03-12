#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
#include <linux/stat.h>
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
	struct buffer_head	*bh;
	struct mfs_inode	*minode;
	struct inode		*inode;
	unsigned long		block, offset;

	inode = iget_locked(sb, ino);
	if (!inode) {
		return ERR_PTR(-ENOMEM);
	}
	if (!(inode->i_state & I_NEW)) {
		return inode;
	}
	if (ino < MFS_ROOT_INODE && ino > MFS_MAX_INODE) {
		return ERR_PTR(-EINVAL);
	}

	/*
	 * Calculate block number and offset within block.
	 */

	block = (ino / 4) + MFS_ILIST_BLOCK;
	offset = ino % 4;

	/*
	 * Read inode from disk and copy in in-memory inode.
	 */

	if (!(bh = sb_bread(sb, block))) {
		return ERR_PTR(-EIO);
	}
	minode = (struct mfs_inode *) bh->b_data; 
	minode += offset;

	/*
	 * ToDo : Copy content to in-memory inode (inode) from inode on device (minode)
	 */

	/* temp arrangemt */
	inode->i_mode = S_IFDIR;
	/* temp arrangemt end*/

	brelse (bh);
	unlock_new_inode(inode);
	return inode;
}
