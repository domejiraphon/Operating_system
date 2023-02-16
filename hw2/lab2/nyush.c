/*
I read STDOUT from this manual. https://en.cppreference.com/w/cpp/io/c/std_streams
I read how to concatenate string https://cplusplus.com/reference/cstring/strcat/
https://www.programiz.com/c-programming/library-function/string.h/strcmp
https://stackoverflow.com/questions/12510874/how-can-i-check-if-a-directory-exists
https://www.geeksforgeeks.org/signals-c-language/
https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
https://www.gnu.org/software/libc/manual/html_node/Basic-Signal-Handling.html
https://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory/
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
  const char* cmd = argv[0];
  if (strcmp(cmd, "cd") == 0){
    int length = getLengthDoublePtr(argv);
    if (length == 1 || length > 2){
      printf("Error: invalid command\n");
      fflush(stderr);
    }
    else{
      //const char *path = "/2250";//
      const char *path = argv[1];
      DIR *dir = opendir(path);
      //printf("%s\n", path);
      //printf("%d", (int)strlen(path)); exit(0);
      if (dir)
        chdir(path);
      else{
        printf("Error: invalid directory\n");
        fflush(stderr);
      }
    }
    return true;
  }
  return false;
}

bool exitTerm(char **argv){
  const char* cmd = argv[0];
  if (strcmp(cmd, "exit") == 0){
    int length = getLengthDoublePtr(argv);
    if (length > 2){
      printf("Error: invalid command\n");
      fflush(stderr);
    }
    exit(0);
    return true;
  }
  return false;
}

void reDirect(char **argv){
  int moreIdx=0;
  int argc = getLengthDoublePtr(argv);
  bool outDirect = false;
  for (; moreIdx < argc; moreIdx++){
    if (strcmp(argv[moreIdx], ">") == 0 ||
        strcmp(argv[moreIdx], ">>") == 0){
      outDirect = true;
      break;
    }
  }
  if (outDirect){
    char *file = argv[moreIdx + 1];
    if (!file){
      printf("Error: invalid command\n");
      fflush(stderr);
      exit(0);
    }
    int fd;
    if (strcmp(argv[moreIdx], ">") == 0){
      fd = open(file, 
          O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    }
    else if (strcmp(argv[moreIdx], ">>") == 0) {
      fd = open(file, 
          O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    }
    dup2(fd, 1);
    close(fd);
  }
  char **argv1=(char **)malloc((moreIdx+1) * sizeof(char *));
  int i=0;
  int skip=0;
  
  for (; i<moreIdx; i++){
    if (strcmp(argv[i], "<") == 0){
      skip++;
      const char *file = argv[i + 1];
      if (!file){
        printf("Error: invalid command\n");
        fflush(stderr);
        exit(0);
      }
      if (access(file, F_OK) != 0){
        printf("Error: invalid file\n" );
        fflush(stderr);
        exit(0);
      }
      continue;
    }
    argv1[i-skip] = argv[i];
  }
  
  argv1[moreIdx - skip] = NULL;
  bool slashExist=false;
  int length = strlen(argv1[0]);
  for (int i=0; i<length && !slashExist; i++){
    if (argv1[0][i] == '/')
      slashExist = true;
  }
  char *pathToLib=NULL;
  char *program=NULL;
  pathToLib = (char *)(malloc(SIZE_MAX * sizeof(char)));
  char *tmp = (char *)(malloc(SIZE_MAX * sizeof(char)));
  strcpy(tmp, argv1[0]);
  if (argv1[0][0] == '/'){
    strcpy(pathToLib, argv1[0]);
    for (int i=length - 1; i >= 0; i--){
      if (argv1[0][i] == '/'){
        pathToLib[i] = '\0';
        break;
      }
    }
    
    char *saveptr=tmp;
    char *token = tmp;
    
    for (; ;tmp = NULL) {
      token = strtok_r(tmp, "/", &saveptr);
      if (!token)
        break;
      program = token;
    }

  }
  else if (slashExist){
    getcwd(pathToLib, SIZE_MAX);
    int last=length-1;
    for (; last>=0 && tmp[last] != '/'; last--)
      ;
    char sl = '/';
    strncat(pathToLib, &sl, 1);
    for (int i=0; i<length; i++)
      if (i < last)
        strncat(pathToLib, &tmp[i], 1);
        //strcat(pathToLib, tmp[i]);
    char *saveptr=tmp;
    char *token = tmp;
    
    for (; ;tmp = NULL) {
      token = strtok_r(tmp, "/", &saveptr);
      if (!token)
        break;
      program = token;
    }

  }
  else {
    strcpy(pathToLib, "/usr/bin");
    program = tmp;
  }
  
  
  DIR *lib = opendir(pathToLib);
  struct dirent *libFile = NULL;
  strcat(pathToLib, "/");
  strcat(pathToLib, program);
  argv1[0] = pathToLib;
  
  
  while ((libFile = readdir(lib))){
    //printf("%s\n", libFile ->d_name);
    if (!strcmp(libFile->d_name, program)){
      execv(argv1[0], argv1);
      free(argv1);
      free(pathToLib);
      free(tmp);
      return;
    }
  }
  printf("Error: invalid program\n");
  fflush(stderr);
  
}
void nextRound(){
  //printf("\n");
  //fflush(stdout);
}
void execute(char **argv, char *lineCmd, struct queue Q){
  pid_t childPid = fork();
  
  if (childPid == 0) {
    
    
    reDirect(argv);
    exit(0);
    //exit(-1);
  } 
  else {
    // parent
    
    int status=0;
    signal(SIGINT, nextRound);
    signal(SIGTSTP, nextRound);
    signal(SIGQUIT, nextRound);
 
    waitpid(childPid, &status, WUNTRACED);
    
    if (WIFSTOPPED(status) == 1){
      push(Q, lineCmd);
    }
  }
}

bool checkJob(char **argv, struct queue Q){
  const char* cmd = argv[0];
  bool out = strcmp(cmd, "jobs") == 0;
  if (out){
    for (int i=0; i<100 && Q.arr[i+Q.start]; i++){
      printf("[%d] %s", i+1, Q.arr[Q.start+i]);
      fflush(stdout);
    }
  }
  return out;
}

void prompt(){
  
  struct queue Q;
  Q.start=0;
  Q.curSize = 0;
  Q.arr = (char **)malloc((100) * sizeof(char *));
  
  while (true) {
    //print terminal current directory
    header();
    
    //read and parse Command
    
    char *lineCmd = readLine();
    if (!lineCmd)
      break;
    char **argv = parsingArgv(lineCmd);
    
    if (!changeDir(argv) &&
        !exitTerm(argv) && 
        !checkJob(argv, Q)){
      
      execute(argv, lineCmd, Q);
    }
    
    //free(lineCmd);
    //free_copied_args(argv, NULL);
    
  }
}

int main() {
  
  prompt();
  
  return 0;
}
