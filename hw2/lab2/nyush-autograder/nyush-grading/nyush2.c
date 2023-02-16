/*
I read STDOUT from this manual. https://en.cppreference.com/w/cpp/io/c/std_streams
I read how to concatenate string https://cplusplus.com/reference/cstring/strcat/
https://www.programiz.com/c-programming/library-function/string.h/strcmp
https://stackoverflow.com/questions/12510874/how-can-i-check-if-a-directory-exists
https://www.geeksforgeeks.org/signals-c-language/
https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
https://www.gnu.org/software/libc/manual/html_node/Basic-Signal-Handling.html
*/
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <stdarg.h>

#include <dirent.h>
#include<signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "utils.h"

#define SIZE_MAX 1000
#define BASEDIR "[nyush "
char *readLine(){
  char *lineCmd = (char *)malloc((SIZE_MAX) * sizeof(char));
  size_t size = SIZE_MAX;
  if ((getline(&lineCmd, &size, stdin) == -1))
    return NULL;
  
  return lineCmd;
}

void header(){
  char *buf = (char *)malloc((SIZE_MAX) * sizeof(char));
  getcwd(buf, SIZE_MAX);
  
  char *saveptr=buf;
  char *token = buf;
  
  char *last = "/";
  for (; ; buf = NULL) {
    token = strtok_r(buf, "/", &saveptr);
    if (!token)
      break;
    last = token;
  }
  char *base = (char *)malloc((SIZE_MAX) * sizeof(char));
  
  strcpy(base, BASEDIR);
  strcat(base, last);
  strcat(base, "]$ ");
  printf("%s", base);
  fflush(stdout);
  free(base);
}

int numArg(const char *lineCmd){
  if (!lineCmd)
    return 0;
  int argc=0;
  const char *delim = " ";
  
  char *str= (char *)malloc((strlen(lineCmd) + 1) * sizeof(char));
  strcpy(str, lineCmd);
  char **saveptr=&str;
  
  while (strtok_r(str, delim, saveptr))
    argc++;
  
  return argc;
}

char **parsingArgv(char *lineCmd){
  
  int argc = numArg(lineCmd);
  exit(0);
  char *str= (char *)malloc((strlen(lineCmd) + 1) * sizeof(char));
  strcpy(str, lineCmd);
  
  char **argv = (char **)malloc((argc) * sizeof(char *));
  
  char *saveptr=str;
  char *token = lineCmd;
  const char *delim = " ";
  
  for (int j=0; ; j++, str = NULL) {
    token = strtok_r(str, delim, &saveptr);
    if (!token)
      break;
    
    int stringLength = strlen(token);
    if (j == argc - 1)
      stringLength--;
    argv[j] = (char *)malloc((stringLength + 1) * sizeof(char));
    strcpy(argv[j], token);
    argv[j][stringLength] = '\0';
  }
  /*
  for (int i=0; i<argc; i++){
    printf("%d, %s\n", (int) strlen(argv[i]), argv[i]);
  }
  */
  argv[argc] = NULL;
  return argv;
}

bool exitTerm(char **argv){
  const char* cmd = argv[0];
  if (strcmp(cmd, "exit") == 0){
    int length = getLengthDoublePtr(argv);
    if (length != 1){
      printf("Error: invalid command\n");
      fflush(stderr);
    }
    return true;
  }
  return false;
}


void prompt(){
  int i=0;
  while (true) {
    //print terminal current directory
    header();
    
    //read and parse Command
    
    char *lineCmd = readLine();
    printf("%s", lineCmd);
    
    char **argv = parsingArgv(lineCmd);
    exit(0);
    if (i == 0)
      break;
    if (exitTerm(argv))
      break;
   
    free(lineCmd);
    free_copied_args(argv, NULL);
    
  }
}

int main() {
  
  prompt();
  
  return 0;
}
