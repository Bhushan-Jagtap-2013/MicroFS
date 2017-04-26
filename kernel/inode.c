#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/stat.h>
#include "../mfs.h"

extern const struct file_operations mfs_dir_ops;
extern const struct inode_operations mfs_dir_inops;
extern struct kmem_cache *mfs_inode_cachep;
extern inline struct mfs_inode_info *GET_MFS_INODE(struct inode *);


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

inline struct mfs_inode_info *GET_MFS_INODE(struct inode *inode) {
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

	printk(KERN_EMERG "MicroFS:: %s", __func__);
	inode = iget_locked(sb, ino);
	dump_stack();
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
	 * allocate appropriate ops to inode
	 */

	if (inode->i_mode & S_IFDIR) {
		printk(KERN_EMERG "MicroFS:: initializing dir ops ");
		inode->i_fop = &mfs_dir_ops;
		inode->i_op = &mfs_dir_inops;
	}

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

/* 
 * Get first free inode to allocate new indoe
 */

int get_inode_number(struct super_block *sb) {
	struct buffer_head	*bh;
	struct mfs_inode_map	*mfs_map;
	int 			i;

	printk(KERN_EMERG "MicroFS:: %s", __func__);
	bh = sb_bread(sb, INODE_MAP_BLK);
	mfs_map = (struct mfs_inode_map *)bh->b_data;
	for(i = 0; i < MFS_MAX_NUM_INODE; i++) {
		if(mfs_map->map[i] == UNUSED) {
			mfs_map->map[i] = USED;
			mark_buffer_dirty(bh);
			brelse(bh);
			return i;
		}
	}	
	brelse(bh);
	return -1;
}

/*
 * mfs_alloc_inode will be called to get in-memory inode from get_locked.
 */

struct inode *mfs_alloc_inode(struct super_block *sb) {
	struct mfs_inode_info	*im_inode;

	printk(KERN_EMERG "MicroFS:: %s", __func__);

	/*
	 * get inode from slab
	 */

	printk(KERN_EMERG "MicroFS:: Calling %s ", __func__);
	im_inode = kmem_cache_alloc(mfs_inode_cachep, GFP_KERNEL);
	if (!im_inode) {
		return NULL;
	}
        return &im_inode->vfs_inode;
}

void mfs_destroy_inode(struct inode *inode) {
	
	/*
	 * ToDo : Provide implementation
	 */

	printk(KERN_EMERG "MicroFS:: Implementation for %s is not provided", __func__);
}

int mfs_write_inode(struct inode *inode, struct writeback_control *wbc) {

	/*
	 * ToDo : Provide implementation
	 */

	printk(KERN_EMERG "MicroFS:: Implementation for %s is not provided", __func__);
	return 0;	/* remove this */
}
	
int mfs_drop_inode(struct inode *inode) {

	/*
	 * ToDo : Provide implementation
	 */

	printk(KERN_EMERG "MicroFS:: Implementation for %s is not provided", __func__);
	return 0;	/* remove this */
}

void mfs_evict_inode(struct inode *inode) {
	struct buffer_head		*bh;
	struct mfs_super_block_info	*msbi;
	struct super_block		*sb = inode->i_sb;
	struct mfs_inode		*minode;
	struct mfs_inode_map		*mfs_map;
	unsigned long			block, offset;
	int				ino = inode->i_ino;

	printk(KERN_EMERG "MicroFS:: %s", __func__);
        truncate_inode_pages_final(&inode->i_data);
        invalidate_inode_buffers(inode);
        clear_inode(inode);
        if (inode->i_nlink) {
                return;
	}

	/*
	 * find and clear corresponding inode from device
	 */

	block = (ino / 4) + MFS_ILIST_START_BLOCK_NUM;
	offset = ino % 4;
	bh = sb_bread(sb, block);
	minode = (struct mfs_inode *) bh->b_data;
	minode += offset;
	memset(minode, 0, sizeof(struct mfs_inode));
	mark_buffer_dirty(bh);
	brelse(bh);

	/*
	 * increase inode free count
	 */

	msbi = sb->s_fs_info;
	msbi->sbi_msb->msb_n_free_inode++;

	/*
	 * ToDo: take care of reducing msb_n_free_blks, while implementing regular files
	 */

	/*
	 * mark inode free
	 */

	bh = sb_bread(sb, INODE_MAP_BLK);
	mfs_map = (struct mfs_inode_map *)bh->b_data;
	mfs_map->map[ino] = UNUSED;
	mark_buffer_dirty(bh);
	brelse(bh);
}
