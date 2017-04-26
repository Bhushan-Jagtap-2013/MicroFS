# MicroFS
μFS is a simple file system written in C for Linux Kernel 4.9.6

# How to build

1. to compile code go to respective directory i.e. cmds or kernel
2. run "make"
3. it will create module mfs.ko
4. insert it in kernel using "insmod mfs.ko"
5. remove module from kernel using "rmmod mfs.ko"
