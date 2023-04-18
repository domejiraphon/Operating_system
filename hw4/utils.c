#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "utils.h"

void printUsage(){
  fprintf(stderr, "Usage: ./nyufile disk <options>\n"
                      "  -i                     Print the file system information.\n"
                      "  -l                     List the root directory.\n"
                      "  -r filename [-s sha1]  Recover a contiguous file.\n"
                      "  -R filename -s sha1    Recover a possibly non-contiguous file.\n");
  exit(EXIT_FAILURE);
}

void parsingName2(char *str, char *filename, char *filetype, const char *delim){
  char *token;
  char *rest= (char *)malloc((strlen(str) + 1) * sizeof(char));
  strcpy(rest, str);
  bool isFirst = true;
  
  while ((token = strtok_r(rest, delim, &rest))){
    if (isFirst){
      strcpy(filename, token + 1);
      isFirst = false;
    }
    else
      strcpy(filetype, token);
  }
}

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

unsigned int getNextBlock(unsigned int *fat, unsigned int curBlock){
  return *(fat + curBlock);
}
