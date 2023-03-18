/*
https://stackoverflow.com/questions/9966663/1-byte-unsigned-integer-c
https://www.programmingsimplified.com/c-program-read-file
https://cplusplus.com/reference/cstdio/fread/
https://stackoverflow.com/questions/48367022/c-iterate-through-char-array-with-a-pointer
https://stackoverflow.com/questions/13975760/split-files-into-fixed-size-blocks-in-c
https://gist.github.com/marcetcheverry/991042
https://www.youtube.com/watch?v=_n2hE2gyPxU&ab_channel=CodeVault
*/
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <getopt.h>
#include "utils.h"
#define SIZE_MAX 100000
int num=0;

char *readFile(char *file){
  int fd = open(file, O_RDONLY);
  if (fd == -1)
    printf("Error openning the file");
  struct stat sb;
  if (fstat(fd, &sb) == -1)
    printf("Error getting file information");
  char *content = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (content == MAP_FAILED)
    printf("mmap failed");
  close(fd);
  num = sb.st_size / sizeof(char);
  return content;
}

void runLenthEncoding(int* idx, char *content, char *result){
  int rep=0;
  if (*idx > 0 && content[0] == result[*idx - 2]){
    rep = result[*idx - 1];
    (*idx) -= 2;
  }
  
  for (int i=0; i<= num; i++){
    if ((i == num) || (i != 0 && content[i] != content[i-1])){
      result[(*idx)++] = content[i-1];
      result[(*idx)++] = rep;
      rep = 1;
    }
    else
      rep++;
    
  }
  
}

void encoder(int argc, char **argv){
  char *content=NULL;
  char *result = (char *)malloc(sizeof(char) * SIZE_MAX);
  int idx = 0;
  for (int i=1; i<argc && strcmp(argv[i], ">"); i++){
    content = readFile(argv[i]);
    runLenthEncoding(&idx, content, result);
  }

  write(1, result, idx);
  free(result);
  
}

int main(int argc, char **argv) {
  int jobs = 1, opt;
  while ((opt = getopt(argc, argv, "j:")) != -1) {
    switch (opt) {
    case 'j':
      jobs = *optarg - '0';
      break;
    default:
      fprintf(stderr, "Usage: %s [-j jobs] file ...\n");
      exit(EXIT_FAILURE);
    }
  }
  
  encoder(argc, argv);
  return 0;
}