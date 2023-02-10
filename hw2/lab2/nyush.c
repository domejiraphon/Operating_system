/*
I read STDOUT from this manual. https://en.cppreference.com/w/cpp/io/c/std_streams
*/
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define SIZE_MAX 1000
#define BASEDIR "[nyush "

void getBaseName(char *buf, char *out){
  int stringLength = strlen(buf);
  int cur = strlen(out);
  int k = cur;
  if (stringLength == 1 && buf[0] == '/'){
    out[k++] = '/';
    out[k++] = ']';
    out[k++] = '$';
    out[k++] = ' ';
    out[k] = '\0';
    return;
  }
  
  for (int i=0; i < stringLength; i++){
    if (buf[i] == '/'){
      k = cur; continue;
    }
    out[k++] = buf[i];
  }
  out[k++] = ']';
  out[k++] = '$';
  out[k++] = ' ';
  out[k] = '\0';
  
}

void prompt(){
  char *curPath;
  char *base;
  char *lineCmd;
  while (true) {
    curPath = (char *)malloc((SIZE_MAX) * sizeof(char));
    base = (char *)malloc((SIZE_MAX) * sizeof(char));
    lineCmd = (char *)malloc((SIZE_MAX) * sizeof(char));
    getcwd(curPath, SIZE_MAX);
    strcpy(base, BASEDIR);
    getBaseName(curPath, base);
    fprintf(stdout, base);
    size_t size = SIZE_MAX;
    getline(&lineCmd, &size, stdin);
    //printf("%s", lineCmd); 
    free(curPath);
    free(base);
    free(lineCmd);
  }
  
  //printf("\n");
  //
}

int main(int argc, char **argv) {
  
  prompt();
  return 0;
}
