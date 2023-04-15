/*
https://stackoverflow.com/questions/6449935/increment-void-pointer-by-one-byte-by-two
https://gist.github.com/Gydo194/a0cec0ba27109a3f5d6317356fed8473
https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
https://www.geeksforgeeks.org/bitwise-operators-in-c-cpp/
https://stackoverflow.com/questions/9284420/how-to-use-sha1-hashing-in-c-programming
https://www.tutorialspoint.com/c_standard_library/c_function_memcpy.htm
https://stackoverflow.com/questions/30146358/how-would-i-compare-2-unsigned-char-arrays
https://www.tutorialspoint.com/c_standard_library/c_function_memcmp.htm
https://stackoverflow.com/questions/3408706/hexadecimal-string-to-byte-array-in-c
https://cplusplus.com/reference/cstring/memcpy/
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
#define SHA_DIGEST_LENGTH 20

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
  //printf("Root cluser: %d\n", boot -> BPB_RootClus);
}

int listDir(int numBlock, void *fs, BootEntry *boot);

unsigned int getOffsetToFAT(BootEntry *boot){
  return boot -> BPB_RsvdSecCnt * boot -> BPB_BytsPerSec;
}

unsigned int getOffsetToData(BootEntry *boot, int num){
  return (boot -> BPB_RsvdSecCnt + boot -> BPB_NumFATs * boot -> BPB_FATSz32
          + (num - 2) * boot -> BPB_SecPerClus) * boot -> BPB_BytsPerSec;
}

unsigned int getStartBlock(DirEntry *dirEntry){
  return (dirEntry -> DIR_FstClusHI << 16) | dirEntry -> DIR_FstClusLO;
}

void listRootDir(char *disk){
  void *fs = readFileSystem(disk);
  BootEntry *boot = (BootEntry *)fs;
  unsigned int offset = getOffsetToFAT(boot);
  void *fat = ((char *) fs) + offset;
  
  int curBlock = boot -> BPB_RootClus;
  int numEntry= 0;
  
  while (curBlock < 0x0ffffff8) {
    numEntry += listDir(curBlock, fs, boot);
    curBlock = *((int *) fat + curBlock);
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
  unsigned int offset = getOffsetToData(boot, numBlock);
  //write(1, ((char *)fs) + offset, boot -> BPB_BytsPerSec);
  int numEntry=0;
  DirEntry *dirEntry = (DirEntry *) ((char *)fs + offset);
  int count=0;
  int maxEntry = (boot -> BPB_BytsPerSec * boot -> BPB_SecPerClus) / DIRENTRY_SIZE;
  
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

void setFAT(void *fs, BootEntry *boot, DirEntry *dirEntry){
  unsigned int offsetToFat = getOffsetToFAT(boot);
  unsigned int *fat = (unsigned int *)((char *) fs + offsetToFat);
  
  unsigned int startBlock = getStartBlock(dirEntry);
  unsigned int blockSize = boot -> BPB_BytsPerSec * boot -> BPB_SecPerClus;

  int leftFileSize = dirEntry -> DIR_FileSize;
  while (leftFileSize > 0){
    *(fat + startBlock) = (leftFileSize <= blockSize) 
          ? EOF: startBlock + 1;
    startBlock++;
    leftFileSize -= blockSize;
  }
}

bool sameSHA(void *fs, BootEntry *boot, DirEntry *dirEntry, char *sha){
  unsigned int offsetToFat = getOffsetToFAT(boot);
  

  unsigned int *fat = (unsigned int *)((char *) fs + offsetToFat);
  

  unsigned char *fileContent = (unsigned char *)(malloc(sizeof(unsigned char) * dirEntry -> DIR_FileSize));
  
  //int fileSize = dirEntry -> DIR_FileSize;
 
  //offsetToBlock = 4608;
  //printf("%d\n", offsetToBlock);
  
  int leftFileSize = dirEntry -> DIR_FileSize;
  unsigned int startBlock = getStartBlock(dirEntry);
  unsigned int blockSize = boot -> BPB_BytsPerSec * boot -> BPB_SecPerClus;
  int idx=0;
  while (leftFileSize > 0){
    
    unsigned int offsetToData = getOffsetToData(boot, startBlock++);
   
    unsigned char *data = (unsigned char *)((char *) fs + offsetToData);
    unsigned int copySize = leftFileSize < blockSize ? leftFileSize : blockSize;
    memcpy(fileContent + idx * blockSize, 
          data, 
          copySize);
    idx++;
    leftFileSize -= copySize;
  }
  
  
  unsigned char md[SHA_DIGEST_LENGTH];
  SHA1(fileContent, dirEntry->DIR_FileSize, md);
  
  unsigned char shaByte[SHA_DIGEST_LENGTH];
  for (size_t i = 0; i < SHA_DIGEST_LENGTH; i++, sha += 2) 
    sscanf(sha, "%2hhx", &shaByte[i]);
  //printf("%s\n", fileContent);
  /*
  printf("%s\n", md);
  printf("%s\n", shaByte);
  printf("%d\n", memcmp(md, shaByte, SHA_DIGEST_LENGTH));
  exit(0);
  */
  if (memcmp(md, shaByte, SHA_DIGEST_LENGTH) == 0)
    return true;
  
  return false;
}

void recoveryFile(char *disk, char *fileToRecover, char *sha){
  void *fs = readFileSystem(disk);
  BootEntry *boot = (BootEntry *)fs;
  unsigned int offset = getOffsetToData(boot, boot -> BPB_RootClus);
  
  DirEntry *dirEntry = (DirEntry *) ((char *)fs + offset);
  int count=0;
  int maxEntry = (boot -> BPB_BytsPerSec * boot -> BPB_SecPerClus) / DIRENTRY_SIZE;
  DirEntry *tmp = NULL;
  while (dirEntry -> DIR_Name[0] != 0x00 && count < maxEntry){
    count++;
    if (dirEntry -> DIR_Attr != 0x10 
        &&dirEntry -> DIR_Name[0] == 0xE5 
        && checkSameFile(dirEntry, fileToRecover)){
      
      if (!sha && tmp){
        printf("%s: multiple candidates found\n", fileToRecover);
        return;
      }
      if (sha){
        if (sameSHA(fs, boot, dirEntry, sha)){
          tmp = dirEntry;
          break;
        }
      }
      else
        tmp = dirEntry;
    }
    dirEntry++;
  }
  
  if (tmp){
    tmp -> DIR_Name[0] = fileToRecover[0];
    setFAT(fs, boot, tmp);
    if (sha)
      printf("%s: successfully recovered with SHA-1\n", fileToRecover);
    else
      printf("%s: successfully recovered\n", fileToRecover);
    return;
  }
  printf("%s: file not found\n", fileToRecover);
}

int main(int argc, char **argv) {
  int opt;
 
  char *filenameCont=NULL;
  char *filenameNonCont=NULL;
  char *sha=NULL;
  if (argc < 3)
    printUsage();
  char *disk = argv[1];
  
  while ((opt = getopt(argc, argv, "ilr:s:R:")) != -1) {
    switch (opt) {
    case 'i':
      if (argc != 3)
        printUsage();
      printFileSystem(disk);
      break;
    case 'l':
      if (argc != 3)
        printUsage();
      listRootDir(disk);
      break;
    case 'r':
      if (argc != 4 && argc != 6)
        printUsage();
      filenameCont = optarg;
      break;
    case 's':
      if (argc !=6)
        printUsage();
      sha = optarg;
      break;
    case 'R':
      if (argc !=6)
        printUsage();
      filenameNonCont = optarg;
      break;
    default:
      printUsage();
    }
  }
  if (filenameCont)
    recoveryFile(disk, filenameCont, sha);
  
  return 0;
}
