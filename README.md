# MicroFS
μFS is a simple file system written in C for Linux Kernel 4.9.6

# Project Report

[Link to project Report](https://github.com/Bhushan-Jagtap-2013/MicroFS/blob/master/Project_Report.pdf)

# How to build

1. to compile code go to respective directory i.e. cmds or kernel
2. run "make"
3. it will create module mfs.ko
4. insert it in kernel using "insmod mfs.ko"
5. remove module from kernel using "rmmod mfs.ko"


# Features Supported

1. Supports up to 1024 blocks of size 1024 bytes each
2. Supports directory name of up to 14 characters
3. Supports maximum 64 directory entries
4. Allows creation and removal of up to 1024 files
5. Maintains time of creation, deletion and modification of files
6. Supports access permissions (Read - Write - Execute - owner)
