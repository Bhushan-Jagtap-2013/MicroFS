# MicroFS
μFS is a simple file system written in C for Linux Kernel 4.9.6

# Directory Structure

commands :  Supporting commnad related to file system
source : Source code of MicroFS
test : Test cases written

# How to build

1. go to source directory
2. run "make"
3. it will create module mfs.ko
4. insert it in kernel using "insmod mfs.ko"
5. remove module from kernel using "rmmod mfs.ko"
