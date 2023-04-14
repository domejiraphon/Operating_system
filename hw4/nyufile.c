/*
https://stackoverflow.com/questions/6449935/increment-void-pointer-by-one-byte-by-two
https://gist.github.com/Gydo194/a0cec0ba27109a3f5d6317356fed8473
https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
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
#define FAT_ADDRESS 32

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
int listDir(int numBlock, void *fs, BootEntry *boot);

void listRootDir(char *disk){
  void *fs = readFileSystem(disk);
  BootEntry *boot = (BootEntry *)fs;
  int offset = boot -> BPB_RsvdSecCnt * boot -> BPB_BytsPerSec;
  void *fat = ((char *) fs) + offset;
  
  int curBlock = boot -> BPB_RootClus;
  
  int numEntry= 0;
  
  while (curBlock < 0x0ffffff8) {
    numEntry += listDir(curBlock - 2, fs, boot);
    curBlock = *((int *) fat + (curBlock - 2));
  }
  printf("Total number of entries = %d\n", numEntry);
}

void parsingName(unsigned char *dirName, char *filename, 
                char *filetype, bool skip, const char *delim){

  for (int i=(skip) ? 1 : 0; 
      i<FILENAME_SIZE && dirName[i] != *delim; i++)
    filename[i - skip] = dirName[i]; 
  
  for (int i=FILENAME_SIZE; i<FILENAME_SIZE + FILETYPE_SIZE 
      && dirName[i] != *delim; i++)
    filetype[i - FILENAME_SIZE] = dirName[i];
}

int listDir(int numBlock, void *fs, BootEntry *boot){
  
  // (num_reserved + num_fat * fat_size + root_cluster) * bytes/cluster
  
  int offset = (boot -> BPB_RsvdSecCnt
                + boot -> BPB_NumFATs * boot -> BPB_FATSz32
                + numBlock) * boot -> BPB_BytsPerSec;
  //write(1, ((char *)fs) + offset, boot -> BPB_BytsPerSec);
  int numEntry=0;
  DirEntry *dirEntry = (DirEntry *) ((char *)fs + offset);
  int count=0;
  int maxEntry = boot -> BPB_BytsPerSec / DIRENTRY_SIZE;
  while (dirEntry -> DIR_Name[0] != 0x00 && count < maxEntry){
    count++;
    if (dirEntry -> DIR_Name[0] == 0xE5){
      dirEntry++;
      continue;
    }
    char filename[FILENAME_SIZE + 1] = {'\0'};
    char filetype[FILETYPE_SIZE + 1] = {'\0'};
  
    parsingName(dirEntry -> DIR_Name, filename, filetype, false, " ");

    if (dirEntry -> DIR_Attr != 0x10 && dirEntry -> DIR_FileSize){
      if (filetype[0] != '\0')
        printf("%s.%s (size = %d, starting cluster = %d)\n", 
            filename,
            filetype,
            dirEntry -> DIR_FileSize,
            dirEntry -> DIR_FstClusLO);
      else 
        printf("%s (size = %d, starting cluster = %d)\n", 
            filename,
            dirEntry -> DIR_FileSize,
            dirEntry -> DIR_FstClusLO);
    }
    else if (dirEntry -> DIR_Attr != 0x10 && !dirEntry -> DIR_FileSize)
      printf("%s (size = 0)\n", filename);
    else
      printf("%s/ (starting cluster = %d)\n", 
            filename,
            dirEntry -> DIR_FstClusLO);
    dirEntry++;
    numEntry++;
  }
  return numEntry;
}

bool checkSameFile(DirEntry *dirEntry, char *fileToRecover){
  char filename[FILENAME_SIZE + 1] = {'\0'};
  char filetype[FILETYPE_SIZE + 1] = {'\0'};
 
  parsingName(dirEntry -> DIR_Name, filename, filetype, true, " ");

  char filenameToRecover[FILENAME_SIZE + 1] = {'\0'};
  char filetypeToRecover[FILETYPE_SIZE + 1] = {'\0'};

  parsingName2(fileToRecover, filenameToRecover, filetypeToRecover, ".");
  /*
  printf("%s\n", filename);
  printf("%s\n", filenameToRecover);
  printf("%s\n", filetype);
  printf("%s", filetypeToRecover);
  exit(0);
  */
  
  if (strcmp(filename, filenameToRecover) == 0
      && strcmp(filetype, filetypeToRecover) == 0)
      return true;
  return false;
}

void recoveryFile(char *disk, char *fileToRecover){
  void *fs = readFileSystem(disk);
  BootEntry *boot = (BootEntry *)fs;
  int offset = (boot -> BPB_RsvdSecCnt
                + boot -> BPB_NumFATs * boot -> BPB_FATSz32
                + boot -> BPB_RootClus - 2) * boot -> BPB_BytsPerSec;
  
  DirEntry *dirEntry = (DirEntry *) ((char *)fs + offset);
  int count=0;
  int maxEntry = boot -> BPB_BytsPerSec / DIRENTRY_SIZE;
  while (dirEntry -> DIR_Name[0] != 0x00 && count < maxEntry){
    count++;
  
    if (dirEntry -> DIR_Name[0] == 0xE5 
        && checkSameFile(dirEntry, fileToRecover)){
      dirEntry -> DIR_Name[0] = fileToRecover[0];
     
      printf("%s: successfully recovered\n", fileToRecover);
      return;
    }
    dirEntry++;
  }
  printf("%s: file not found\n", fileToRecover);
}

int main(int argc, char **argv) {
  int opt;
  bool checkFS=false, checkRoot=false;
  char *filename=NULL;
  if (argc < 2)
    printUsage();
  while ((opt = getopt(argc, argv, "ilr:s:R:")) != -1) {
    switch (opt) {
    case 'i':
      checkFS = true;
      break;
    case 'l':
      checkRoot = true;
      break;
    case 'r':
      filename = optarg;
      break;
    case 's':
      break;
    case 'R':
      break;
    default:
      printUsage();
    }
  }
  if (optind >= argc)
    printUsage();
  if (checkFS)
    printFileSystem(argv[optind]);
  else if (checkRoot)
    listRootDir(argv[optind]);
  else if (filename)
    recoveryFile(argv[optind], filename);
  return 0;
}