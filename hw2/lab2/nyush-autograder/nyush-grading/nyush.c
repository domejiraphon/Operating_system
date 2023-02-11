/*
I read STDOUT from this manual. https://en.cppreference.com/w/cpp/io/c/std_streams
I read how to concatenate string https://cplusplus.com/reference/cstring/strcat/
https://www.programiz.com/c-programming/library-function/string.h/strcmp
https://stackoverflow.com/questions/12510874/how-can-i-check-if-a-directory-exists
*/
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <sys/types.h>
#include <dirent.h>

#include "utils.h"

#define SIZE_MAX 1000
#define BASEDIR "[nyush "

char *readLine(){
  char *lineCmd = (char *)malloc((SIZE_MAX) * sizeof(char));
  size_t size = SIZE_MAX;
  getline(&lineCmd, &size, stdin);
  return lineCmd;
}

void header(){
  char *buf = (char *)malloc((SIZE_MAX) * sizeof(char));
  getcwd(buf, SIZE_MAX);
  char *base = (char *)malloc((SIZE_MAX) * sizeof(char));
  
  strcpy(base, BASEDIR);
  int stringLength = strlen(buf);
  int cur = strlen(base);
  int k = cur;
  if (stringLength == 1 && buf[0] == '/')
    base[k++] = '/';
  else {
    for (int i=0; i < stringLength; i++){
      if (buf[i] == '/'){
        k = cur; continue;
      }
      base[k++] = buf[i];
    }
  }
  free(buf);
  base[k] = '\0';
  const char *end = "]$ ";
  strcat(base, end);
  fprintf(stdout, base);
  free(base);
}

char *parsingCmd(char *lineCmd){
  int lengthCmd=0;
  int lengthEntire = strlen(lineCmd);
  
  for (; lengthCmd < lengthEntire - 1&& lineCmd[lengthCmd] != ' '; lengthCmd++)
    ;
  
  char *file = (char *)malloc((lengthCmd + 1) * sizeof(char));
  for (int i=0; i< lengthCmd; i++)
    file[i] = lineCmd[i];
  file[lengthCmd] = '\0';
  //printf("%s, %d\n", file, (int) strlen(file));
  return file;
}

int numArg(char *lineCmd){
  int argc=0;
  int i=0;
  int lengthEntire = strlen(lineCmd);
  
  while (i < lengthEntire - 1){
    while (i < lengthEntire - 1 && lineCmd[i] == ' ')
      i++;
    while (i < lengthEntire - 1&& lineCmd[i] != ' ')
      i++;
    if (i > lengthEntire - 1)
      break;
    argc++;
  }
  
 
  return argc;
}

char **parsingArgv(char *lineCmd){
  int lengthEntire = strlen(lineCmd);
  int argc = numArg(lineCmd);
  int st=0;
  while (lineCmd[st] != ' ')
    st++;
  while (lineCmd[st] == ' ')
    st++;
  
  char **argv = (char **)malloc((argc) * sizeof(char *));
  
  for(int j=0; j < argc-1; j++) {
    int stringLength=0;
    while (st < lengthEntire - 1 && lineCmd[st] == ' ')
      st++;
    int tmp = st;
    while (st < lengthEntire - 1 && lineCmd[st] != ' '){
      stringLength++;
      st++;
    }
    argv[j] = (char *)malloc((stringLength+1) * sizeof(char));
    for (int k=0; k<stringLength; k++)
      argv[j][k] = lineCmd[tmp++];
    argv[j][stringLength] = '\0';
  }
  argv[argc] = NULL;
  return argv;
}

bool changeDir(const char *file, char **argvPtr){
  const char* changeDirCmd = "cd";
  int length = getLengthDoublePtr(argvPtr);
  
  bool out = strcmp(file, changeDirCmd) == 0;
  if (out){
    if (length == 0 || length > 1)
      fprintf(stderr, "Error: invalid command\n");
    else{
      DIR *dir = opendir(argvPtr[0]);
      if (dir)
        chdir(argvPtr[0]);
      else
        fprintf(stderr, "Error: invalid directory\n");
    }
  }
  return out;
}

bool exitTerm(const char *file, char **argvPtr){
  const char* exitCmd = "exit";
  bool out = strcmp(file, exitCmd) == 0;
  int length = getLengthDoublePtr(argvPtr);
  if (out){
    if (length != 0)
      fprintf(stderr, "Error: invalid command\n");
    else
      exit(-1);
  }
    
  return out;
}
void prompt(){
  while (true) {
    //print terminal current directory
    header();
    

    //read and parse Command
    char *lineCmd = readLine();
    const char *file = parsingCmd(lineCmd);
    char **argvPtr = parsingArgv(lineCmd);
    
    int argc = numArg(lineCmd);
    char *argv[argc];
    for (int i=0; i<argc; i++){
      argv[i] = argvPtr[i];
      //printf("%s\n", argv[i]);
    }
    
    if (!changeDir(file, argvPtr) &&
        !exitTerm(file, argvPtr)){
      pid_t pid = fork();
      if (pid == 0) {
        //child
        execvp(file, argv);
        exit(-1);
      } else {
        // parent
        wait(NULL);
      }
    }
    
    free(lineCmd);
    free_copied_args(argvPtr, NULL);
  }
 
}

int main() {
  
  prompt();
  return 0;
}
