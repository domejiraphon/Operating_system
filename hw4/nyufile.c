/*
https://stackoverflow.com/questions/6449935/increment-void-pointer-by-one-byte-by-two
https://gist.github.com/Gydo194/a0cec0ba27109a3f5d6317356fed8473
*/
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <openssl/sha.h>

#include "utils.h"

#define DIRENTRY_SIZE 32
#define FILENAME_SIZE 8
#define FILETYPE_SIZE 3

void *readFileSystem(char *disk){
  int fd;
  struct stat sb;
 
  fd = open(disk, O_RDWR);
  if (fd == -1)
    fprintf(stderr, "Error openning the file");
  if (fstat(fd, &sb) == -1)
    fprintf(stderr, "Error getting file information");
  
  void *fs = mmap(NULL, sb.st_size, 
                      PROT_READ | PROT_WRITE, 
                      MAP_SHARED, fd, 0);
  close(fd);
  if (fs == MAP_FAILED)
    fprintf(stderr, "mmap failed");
  return fs;
}

void printFileSystem(char *disk){
  BootEntry *boot = (BootEntry *) readFileSystem(disk);
  printf("Number of FATs = %d\n", boot -> BPB_NumFATs);
  printf("Number of bytes per sector = %d\n", boot -> BPB_BytsPerSec);
  printf("Number of sectors per cluster = %d\n", boot -> BPB_SecPerClus);
  printf("Number of reserved sectors = %d\n", boot -> BPB_RsvdSecCnt);
}

void listRootDir(char *disk){
  void *fs = readFileSystem(disk);
  BootEntry *boot = (BootEntry *)fs;
  // (num_reserved + num_fat * fat_size + root_cluster) * bytes/cluster
  int offset = (boot -> BPB_RsvdSecCnt
                + boot -> BPB_NumFATs * boot -> BPB_FATSz32
                + boot -> BPB_RootClus - 2) * boot -> BPB_SecPerClus * boot -> BPB_BytsPerSec;
  //write(1, ((char *)fs) + offset, boot -> BPB_BytsPerSec);
 
  DirEntry *dirEntry = (DirEntry *) ((char *)fs + offset);
  while (dirEntry -> DIR_Name[0] != 0x00){
    char filename[FILENAME_SIZE + 1] = {'\0'};
    char filetype[FILETYPE_SIZE + 1] = {'\0'};
    int i=0;
    for (; i<FILENAME_SIZE && dirEntry -> DIR_Name[i] != ' '; i++)
      filename[i] = dirEntry -> DIR_Name[i]; 
  
    for (int i=FILENAME_SIZE; i<FILENAME_SIZE + FILETYPE_SIZE 
        && dirEntry -> DIR_Name[i] != ' '; i++)
      filetype[i - FILENAME_SIZE] = dirEntry -> DIR_Name[i];

    if (dirEntry -> DIR_Attr != 0x10){
      printf("%s", filename);
      if (filetype[0] != '\0')
        printf(".%s", filetype);
    }
    else
      printf("%s/", filename);
    
    printf(" (size = %d, starting cluster = %d", 
          dirEntry -> DIR_FileSize, dirEntry -> DIR_FstClusLO);
    dirEntry++;
    printf("\n");
  }
  
}

int main(int argc, char **argv) {
  int opt;
  bool checkFS=false, checkRoot=false;
  while ((opt = getopt(argc, argv, "ilr:s:R:")) != -1) {
    switch (opt) {
    case 'i':
      checkFS = true;
      break;
    case 'l':
      checkRoot = true;
      break;
    default:
      fprintf(stderr, "Usage: ./nyufile disk <options>\n"
                      "  -i                     Print the file system information.\n"
                      "  -l                     List the root directory.\n"
                      "  -r filename [-s sha1]  Recover a contiguous file.\n"
                      "  -R filename -s sha1    Recover a possibly non-contiguous file.\n");
      exit(EXIT_FAILURE);
    }
  }
  if (checkFS)
    printFileSystem(argv[optind]);
  else if (checkRoot)
    listRootDir(argv[optind]);
  return 0;
}