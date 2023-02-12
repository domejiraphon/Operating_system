/*
I read STDOUT from this manual. https://en.cppreference.com/w/cpp/io/c/std_streams
I read how to concatenate string https://cplusplus.com/reference/cstring/strcat/
https://www.programiz.com/c-programming/library-function/string.h/strcmp
https://stackoverflow.com/questions/12510874/how-can-i-check-if-a-directory-exists
https://www.geeksforgeeks.org/signals-c-language/
https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
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
#include<signal.h>
#include <fcntl.h>

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

int numArg(const char *lineCmd){
  int argc=0;
  const char *delim = " ";
  
  char *str= (char *)malloc((strlen(lineCmd) + 1) * sizeof(char));
  strcpy(str, lineCmd);
  char **saveptr=&str;
  
  while (strtok_r(str, delim, saveptr))
    argc++;
  
  //free(str);
  
  return argc;
}

char **parsingArgv(char *lineCmd){
  
  int argc = numArg(lineCmd);
  
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

bool changeDir(char **argv){
  const char* changeDirCmd = "cd";
  const char* cmd = argv[0];
  bool out = strcmp(cmd, changeDirCmd) == 0;
  if (out){
    int length = getLengthDoublePtr(argv);
    
    if (length == 1 || length > 2)
      fprintf(stderr, "Error: invalid command\n");
    else{
      //const char *path = "/2250";//
      const char *path = argv[1];
      DIR *dir = opendir(path);
      //printf("%s\n", path);
      //printf("%d", (int)strlen(path)); exit(0);
      if (dir)
        chdir(path);
      else
        fprintf(stderr, "Error: invalid directory\n");
    }
  }
  return out;
}

bool exitTerm(char **argv){
  const char* exitCmd = "exit";
  const char* cmd = argv[0];
  bool out = strcmp(cmd, exitCmd) == 0;
  if (out){
    int length = getLengthDoublePtr(argv);
    if (length != 1)
      fprintf(stderr, "Error: invalid command\n");
    else
      exit(-1);
  }
    
  return out;
}

void handleExit(){
  printf("Kill child\n");
}

bool reDirect(char **argv){
  const char* cmd = argv[0];
  bool out = strcmp(cmd, "cat") == 0;
  if (out) {
    bool read = strcmp(argv[1], "<") == 0;
    bool write = strcmp(argv[1], ">") == 0;
    if (write){
      int fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
      dup2(fd, 1);
      close(fd);
      fprintf(stdout, "This will be written to output.txt\n");
    }
    else if (read) {
      int fd = open(argv[2], O_RDONLY);
      dup2(fd, 0);
      close(fd);
      char *out = readLine();
      fprintf(stdout, out);
      free(out);
    }
    //free(argv);
    
  }

  return out;
}
void execute(char **argv){
  pid_t childPid = fork();
  if (childPid == 0) {
    //child
    //signal(SIGINT, handleExit);
    //signal(SIGTSTP, handleExit);
    
    if (!reDirect(argv)){
      execv("/bin/ls", argv);
    }
      
    exit(-1);
  } else {
    // parent
    //kill(childPid, SIGINT);
    //kill(childPid, SIGTSTP);
    int *status=NULL;
    waitpid(childPid, status, 0);
  }
}

void prompt(){
  while (true) {
    //print terminal current directory
    header();
    

    //read and parse Command
    char *lineCmd = readLine();
    char **argv = parsingArgv(lineCmd);
    
    if (!changeDir(argv) &&
        !exitTerm(argv)){
      execute(argv);
    }
    
    //free(lineCmd);
    //free_copied_args(argv, NULL);
  }
}

int main() {
  //signal(SIGINT, prompt);
  //signal(SIGTSTP, prompt);
  
  
  prompt();
  
  return 0;
}
