obj-m += mfs.o
mfs-objs := init.o super.o inode.o dir.o

ccflags-y += -D__KERNEL__

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
