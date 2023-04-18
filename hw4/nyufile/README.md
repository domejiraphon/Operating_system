# Overview
In this lab, you will work on the data stored in the FAT32 file system directly, without the OS file system support. You will implement a tool that recovers a deleted file specified by the user.

## Usage
```
Usage: ./nyufile disk <options>
  -i                     Print the file system information.
  -l                     List the root directory.
  -r filename [-s sha1]  Recover a contiguous file.
  -R filename -s sha1    Recover a possibly non-contiguous file.
```
For example:
```
make
./nyufile fat32.disk -r FILE.TXT -s c91761a2cc1562d36585614c8c680ecf5712e875
```