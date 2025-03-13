# Project 2: File System

This is the starter code for [Project 2](https://course.ccs.neu.edu/cs3650sp23/p2.html). The following contents are provided:

- [Makefile](Makefile)   - Targets are explained in the assignment text
- [README.md](README.md) - This README
- [helpers](helpers)     - Helper code implementing access to bitmaps and blocks
- [hints](hints)         - Incomplete bits and pieces that you might want to use as inspiration
- [nufs.c](nufs.c)       - The main file of the file system driver
- [test.pl](test.pl)     - Tests to exercise the file system

## Running the tests

You might need install an additional package to run the provided tests:

```
$ sudo apt-get install libtest-simple-perl
```

Then using `make test` will run the provided tests.


arsshbajpai@Arsshs-MacBook-Pro p2-arssh-bajarsshbajpai@Arsshs-MacBook-Pro p2-arssh-bajpai % make 
clean
umount mnt || true
umount: mnt: not currently mounted
rm -f nufs *.o test.log data.nufs
rmdir mnt || true
rmdir: mnt: Directory not empty
arsshbajpai@Arsshs-MacBook-Pro p2-arssh-bajpai % rm -rf mnt/*

zsh: sure you want to delete all 7 files in /Users/arsshbajpai/p2-arssh-bajpai/mnt [yn]? y
arsshbajpai@Arsshs-MacBook-Pro p2-arssh-bajpai % ./nufs mnt nufs.mg 
zsh: no such file or directory: ./nufs
arsshbajpai@Arsshs-MacBook-Pro p2-arssh-bajpai % make 
nufs
gcc -g `pkg-config fuse --cflags` -c -o bitmap.o bitmap.c
gcc -g `pkg-config fuse --cflags` -c -o blocks.o blocks.c
gcc -g `pkg-config fuse --cflags` -c -o directory.o directory.c
gcc -g `pkg-config fuse --cflags` -c -o inode.o inode.c
gcc -g `pkg-config fuse --cflags` -c -o nufs.o nufs.c
gcc -g `pkg-config fuse --cflags` -c -o slist.o slist.c
gcc -g `pkg-config fuse --cflags` -c -o storage.o storage.c
gcc  -o nufs bitmap.o blocks.o directory.o inode.o nufs.o slist.o storage.o `pkg-config fuse --libs`
arsshbajpai@Arsshs-MacBook-Pro p2-arssh-bajpai % ./nufs mnt nufs.mg
[INFO] Initializing file system with disk image: nufs.mg
[INFO] Mount point: mnt
[DEBUG] Arg[0]: ./nufs
[DEBUG] Arg[1]: mnt
[DEBUG] Arg[2]: nufs.mg
[INFO] Initializing storage system with file: nufs.mg
[INFO] Storage initialized from nufs.mg.
[INFO] Mounting file system at: mnt
arsshbajpai@Arsshs-MacBook-Pro p2-arssh-bajpai % make 
test
perl test.pl
1..31
#           == Basic Tests ==
not ok 1 - File1 exists.
#   Failed test 'File1 exists.'
#   at test.pl line 51.
not ok 2 - File1 is regular file.
#   Failed test 'File1 is regular file.'
#   at test.pl line 52.
# 'hello, one' eq ''?
not ok 3 - Read back data1 correctly.
#   Failed test 'Read back data1 correctly.'
#   at test.pl line 55.
not ok 4 - File2 exists.
#   Failed test 'File2 exists.'
#   at test.pl line 59.
not ok 5 - File2 is regular file.
#   Failed test 'File2 is regular file.'
#   at test.pl line 60.
# 'hello, one' eq ''?
not ok 6 - Read back data2 correctly.
#   Failed test 'Read back data2 correctly.'
#   at test.pl line 63.
not ok 7 - one.txt is in the directory
#   Failed test 'one.txt is in the directory'
#   at test.pl line 66.
not ok 8 - two.txt is in the directory
#   Failed test 'two.txt is in the directory'
#   at test.pl line 67.
not ok 9 - Read back long correctly.
#   Failed test 'Read back long correctly.'
#   at test.pl line 72.
not ok 10 - Read with offset & length
#   Failed test 'Read with offset & length'
#   at test.pl line 76.
ok 11 - one.txt is not in the directory
ok 12 - two.txt is not in the directory
not ok 13 - one.txt is in the directory still
#   Failed test 'one.txt is in the directory still'
#   at test.pl line 89.
not ok 14 - two.txt is in the directory still
#   Failed test 'two.txt is in the directory still'
#   at test.pl line 90.
# 'hello, one' eq ''?
not ok 15 - Read back data1 correctly again.
#   Failed test 'Read back data1 correctly again.'
#   at test.pl line 94.
# 'hello, two' eq ''?
not ok 16 - Read back data2 correctly again.
#   Failed test 'Read back data2 correctly again.'
#   at test.pl line 98.
# Testing unlink...
ok 17 - deleted one.txt
#           == Advanced Tests ==
# Nested directories
ok 18 - Create a new directory
ok 19 - Create another directory
ok 20 - Directories exist
ok 21 - Directories listed
ok 22 - Create a nested directory
ok 23 - Create a nested-nested directory
ok 24 - Create a file in a directory
ok 25 - Read data back correctly
ok 26 - Move a file to another directory
ok 27 - Read data back correctly
# Large files
# -> 2 blocks
# Actual size: 6145
ok 28 - Large file exists and has the correct size
ok 29 - Read back data from large file correctly
# -> 4 blocks
# Actual size: 14337
ok 30 - Larger file exists and has the correct size
ok 31 - Read back data from larger file correctly
# Looks like you failed 14 tests of 31.
make: *** [test] Error 14
arsshbajpai@Arsshs-MacBook-Pro p2-arssh-bajpai % make 
test
perl test.pl
1..31
#           == Basic Tests ==
ok 1 - File1 exists.
ok 2 - File1 is regular file.
# 'hello, one' eq 'hello, one'?
ok 3 - Read back data1 correctly.
ok 4 - File2 exists.
ok 5 - File2 is regular file.
# 'hello, one' eq 'hello, one'?
ok 6 - Read back data2 correctly.
ok 7 - one.txt is in the directory
ok 8 - two.txt is in the directory
ok 9 - Read back long correctly.
ok 10 - Read with offset & length
one.txt exists after umount; FS never mounted? at test.pl line 80.
# Looks like your test exited with 255 just after 10.
make: *** [test] Error 255
arsshbajpai@Arsshs-MacBook-Pro p2-arssh-bajpai % %
