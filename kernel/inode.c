#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include "../mfs.h"

/*
 * GET_MFS_INODE will return mfs_inode_info structure pointer by encapsulating linux inode structure
 * in it at vxfs_inode location.
 *
 * return starting  address ->  |-struct mfs_inode_info-|
 *                              |                       |
 *                              |       mi_blk_add      |
 *                              |       vfs_inode <-----|---- we need to provide address of this field
 *                              |                       |
 *                              -------------------------
 */

static inline struct mfs_inode_info *GET_MFS_INODE(struct inode *inode) {
	return (container_of(inode, struct mfs_inode_info, vfs_inode));
}

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
	struct mfs_inode_info	*minode_info;
	struct mfs_inode	*minode;
	struct inode		*inode;
	unsigned long		block, offset;
	int			i;

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

	minode_info = GET_MFS_INODE(inode);

	/*
	 * Calculate block number and offset within block.
	 */

	block = (ino / 4) + MFS_ILIST_START_BLOCK_NUM;
	offset = ino % 4;

	printk(KERN_EMERG "MicroFS : block + offset %lu %lu", block, offset);

	/*
	 * Read inode from disk and copy in in-memory inode.
	 */

	if (!(bh = sb_bread(sb, block))) {
		return ERR_PTR(-EIO);
	}
	minode = (struct mfs_inode *) bh->b_data; 
	minode += offset;

	/*
	 * Copy content to in-memory inode (inode) from inode on device (minode)
	 */

	inode->i_mode = minode->mi_mode;
	i_uid_write(inode, minode->mi_uid);
	i_gid_write(inode, minode->mi_gid);
	set_nlink(inode, minode->mi_links_count);
	inode->i_size = minode->mi_size;
	inode->i_atime.tv_sec = (signed)minode->mi_atime;
	inode->i_ctime.tv_sec = (signed)minode->mi_ctime;
	inode->i_mtime.tv_sec = (signed)minode->mi_mtime;
	inode->i_blocks = minode->mi_blocks;
	inode->i_private = kmalloc(sizeof(struct mfs_inode), GFP_KERNEL);

	/* 
	 * store block addresses in private filed of in memory inode
	 */

	for(i = 0; i < MFS_IBLOCK_COUNT; i++) {
		minode_info->mi_blk_add[i] = minode->mi_blk_add[i];
	}
	brelse (bh);
	unlock_new_inode(inode);
	return inode;
}
