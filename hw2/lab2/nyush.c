/*
I read STDOUT from this manual. https://en.cppreference.com/w/cpp/io/c/std_streams
I read how to concatenate string https://cplusplus.com/reference/cstring/strcat/
https://www.programiz.com/c-programming/library-function/string.h/strcmp
https://www.ibm.com/docs/en/zos/2.3.0?topic=functions-getcwd-get-path-name-working-directory
https://stackoverflow.com/questions/12510874/how-can-i-check-if-a-directory-exists
https://www.geeksforgeeks.org/signals-c-language/
https://www.geeksforgeeks.org/strtok-strtok_r-functions-c-examples/
https://www.gnu.org/software/libc/manual/html_node/Basic-Signal-Handling.html
https://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory/
https://www.geeksforgeeks.org/how-to-append-a-character-to-a-string-in-c/
https://stackoverflow.com/questions/39002052/how-i-can-print-to-stderr-in-c
https://stackoverflow.com/questions/19461744/how-to-make-parent-wait-for-all-child-processes-to-finish
https://www.tutorialspoint.com/cprogramming/c_structures.htm
https://www.programiz.com/c-programming/c-structure-function
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

char **parsingArgv(int argc, char *lineCmd, const char *delim){
  
  char *str= (char *)malloc((strlen(lineCmd) + 1) * sizeof(char));
  strcpy(str, lineCmd);
  
  char **argv = (char **)malloc((argc) * sizeof(char *));
  
  char *saveptr=str;
  char *token = lineCmd;
  
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
  
  argv[argc] = NULL;
  return argv;
}

bool changeDir(int argc, char **argv){
  const char* cmd = argv[0];
  if (!strcmp(cmd, "cd")){
    if (argc == 1 || argc > 2){
      fprintf(stderr, "Error: invalid command\n");
      fflush(stderr);
    }
    else{
      const char *path = argv[1];
      DIR *dir = opendir(path);
      if (dir)
        chdir(path);
      else{
        fprintf(stderr, "Error: invalid directory\n");
        fflush(stderr);
      }
    }
    return true;
  }
  return false;
}

bool exitTerm(int argc, char **argv, struct Node *head, struct Node *tail){
  const char* cmd = argv[0];
  if (!strcmp(cmd, "exit")){
    if (argc != 1){
      fprintf(stderr, "Error: invalid command\n");
      fflush(stderr);
      return true;
    }
    else if (!empty(head, tail)){
      fprintf(stderr, "Error: there are suspended jobs\n");
      fflush(stderr);
      return true;
    }
    exit(0);
    return true;
  }
  return false;
}

void locatingProgram(char **argv, int moreIdx){
  char **argv1=(char **)malloc((moreIdx+1) * sizeof(char *));
  int i=0;
  int skip=0;
  
  for (; i<moreIdx; i++){
    if (!strcmp(argv[i], "<")){
      skip++;
      const char *file = argv[i + 1];
      if (!file){
        fprintf(stderr, "Error: invalid command\n");
        fflush(stderr);
        exit(0);
      }
      if (access(file, F_OK) != 0){
        fprintf(stderr, "Error: invalid file\n" );
        fflush(stderr);
        exit(0);
      }
      
      int fd;
      fd = open(file, O_RDONLY, S_IRUSR | S_IWUSR);
      dup2(fd, 0);
      close(fd);
      break;
    }
    argv1[i-skip] = argv[i];
  }
  argv1[i] = NULL;
 
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
  
  
  if (slashExist){
    strcat(pathToLib, "/");
    strcat(pathToLib, program);
    argv1[0] = pathToLib;
  }
  
  while ((libFile = readdir(lib))){
    if (!strcmp(libFile->d_name, program)){
      if (slashExist)
        execv(argv1[0], argv1);
      else
        execvp(argv1[0], argv1);
      free(argv1);
      free(pathToLib);
      free(tmp);
      return;
    }
  }
  fprintf(stderr, "Error: invalid program\n");
  fflush(stderr);
}

bool reDirect(char **argv){
  int moreIdx=0;
  int argc = getLengthDoublePtr(argv);
  for (int i=0; i<argc; i++)
    if (!strcmp(argv[i], "|"))
      return false;
  
  bool outDirect = false;
  for (; moreIdx < argc; moreIdx++){
    if (!strcmp(argv[moreIdx], ">") ||
        !strcmp(argv[moreIdx], ">>")){
      outDirect = true;
      break;
    }
  }
  if (outDirect){
    char *file = argv[moreIdx + 1];
    if (!file){
      fprintf(stderr, "Error: invalid command\n");
      fflush(stderr);
      exit(0);
    }
    int fd;
    if (!strcmp(argv[moreIdx], ">")){
      fd = open(file, 
          O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    }
    else if (!strcmp(argv[moreIdx], ">>")) {
      fd = open(file, 
          O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
    }
    dup2(fd, 1);
    close(fd);
  }
  locatingProgram(argv, moreIdx);
  return true;
}

void pipeExec(char **argv){
  int argc = getLengthDoublePtr(argv);
  int pipeIdx=0;
  
  while (pipeIdx < argc){
    for (; pipeIdx<argc && strcmp(argv[pipeIdx], "|"); pipeIdx++)
      ;
    if (pipeIdx == argc)
      break;
    if (pipeIdx == 0 || pipeIdx == argc-1){
      fprintf(stderr, "Error: invalid command\n");
      fflush(stderr);
      return;
    }
    int fildes[2];
    char **argv1, **argv2;
    pipe(fildes);
    if (!fork()){
      //first
      dup2(fildes[1], 1);
      close(fildes[0]);
      close(fildes[1]);
     
      argv1 = (char **)(malloc((pipeIdx + 1) * sizeof(char *)));
      for (int i=0; i<pipeIdx; i++)
        argv1[i] = argv[i];
      argv1[pipeIdx] = NULL;
   
      reDirect(argv1);
    }
    else {
      //second
      dup2(fildes[0], 0);
      close(fildes[0]);
      close(fildes[1]);
      argv2 = (char **)(malloc((argc - pipeIdx + 1) * sizeof(char *)));
      for (int i=0; i<argc - pipeIdx; i++)
        argv2[i] = argv[i + pipeIdx + 1];
      argv2[argc - pipeIdx] = NULL;
     
      reDirect(argv2);
    }
    free_copied_args(argv1, argv2, NULL);
    pipeIdx++;
    break;
  }
}

bool checkFg(int argc, char **argv, struct Node *head, struct Node* tail){
  const char* cmd = argv[0];
  if (!strcmp(cmd, "fg")){
    if (argc != 2){
      fprintf(stderr, "Error: invalid command\n");
      fflush(stderr);
    }
    else {
      struct Node *resumeProcess = removeNode(head, tail, *argv[1] - '1');
      if (resumeProcess){
        int status=0;
        kill(resumeProcess -> pid, SIGCONT);
        waitpid(resumeProcess -> pid, &status, WUNTRACED);
        if (WIFSTOPPED(status))
          addNode(head, resumeProcess -> cmd, resumeProcess -> pid);
      }
      else{
        fprintf(stderr, "Error: invalid job\n");
        fflush(stderr);
      }
    }
    return true;
  }
  
  return false;
}

bool checkJob(int argc, char **argv, struct Node *head, struct Node *tail){
  const char* cmd = argv[0];
  if (!strcmp(cmd, "jobs")){
    if (argc != 1){
      fprintf(stderr, "Error: invalid command\n");
      fflush(stderr);
    }
    else
      printJobs(head, tail);
    return true;
  }
  return false;
}

void execute(char **argv, char *lineCmd, struct Node *head){
  pid_t childPid = fork();
  if (childPid == 0) {
    if (!reDirect(argv))
      pipeExec(argv);
    
    exit(0);
  } 
  else {
    // parent
    
    int status=0;
    signal(SIGINT, nextRound);
    signal(SIGTSTP, nextRound);
    signal(SIGQUIT, nextRound);
    waitpid(childPid, &status, WUNTRACED);
    
    if (WIFSTOPPED(status))
      addNode(head, lineCmd, childPid);

  }
}

void prompt(){
  
  struct Node *head = (struct Node *)malloc(sizeof(struct Node));
  struct Node *tail = (struct Node *)malloc(sizeof(struct Node));
  head -> next = tail;
  tail -> prev = head;
  while (true) {
    //print terminal current directory
    header();
    
    //read and parse Command
    
    char *lineCmd = readLine();
    if (!lineCmd)
      break;
    int argc = numArg(lineCmd);
    char **argv = parsingArgv(argc, lineCmd, " ");
    
    if (!changeDir(argc, argv) &&
        !exitTerm(argc, argv, head, tail) &&
        !checkFg(argc, argv, head, tail) &&
        !checkJob(argc, argv, head, tail)){
      
      execute(argv, lineCmd, head);
    }
    free_copied_args(argv, NULL);
  }
  clearList(head);
}

int main() {
  prompt();
  return 0;
}
