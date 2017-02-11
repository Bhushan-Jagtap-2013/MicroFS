#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#define FS_AUTHOR "Bhushan Jagtap"
#define FS_DESC "Simple file system module"

static int __init mfs_register(void) {
	printk(KERN_INFO "Register file system");
	return 0;
}

static void __exit mfs_unregister(void) {
	printk(KERN_INFO "Unregister file system");
}

module_init(mfs_register);
module_exit(mfs_unregister);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(FS_AUTHOR);
MODULE_DESCRIPTION(FS_DESC);
