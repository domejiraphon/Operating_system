/*
https://stackoverflow.com/questions/9966663/1-byte-unsigned-integer-c
https://www.programmingsimplified.com/c-program-read-file
https://cplusplus.com/reference/cstdio/fread/
https://stackoverflow.com/questions/48367022/c-iterate-through-char-array-with-a-pointer
*/
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include "utils.h"
#define SIZE_MAX 1000

int readFile(char *file, char *content){
  FILE *fp = fopen(file, "r");
  fseek(fp, 0 , SEEK_END);
  long lSize = ftell(fp);
  rewind(fp);
  char *buffer = (char*)malloc(sizeof(char) * lSize);
  fread(buffer, 1, lSize, fp);
  fclose(fp);
  strcat(content, buffer);
  free(buffer);
  return lSize;
}
void runLenthEncoding(int num, char *content){
  int rep=0;
  char *result = (char *)malloc(sizeof(char) * SIZE_MAX);
  int idx=0;
  for (int i=0; i<=num; i++){
    if ((i == num) || (i != 0 && content[i] != content[i-1])){
      result[idx++] = content[i-1];
      result[idx++] = rep;
      rep = 1;
    }
    else
      rep++;
  }
  result[idx] = '\0';
  printf("%s", result);
  free(result);
}
void encoder(int argc, char **argv){
  
  char *content = (char *)malloc((SIZE_MAX) * sizeof(char));
  int num=0;
  for (int i=1; i<argc && strcmp(argv[i], ">"); i++)
    num += readFile(argv[i], content);
  
  runLenthEncoding(num, content);
  free(content);
}

int main(int argc, char **argv) {
  encoder(argc, argv);
  return 0;
}