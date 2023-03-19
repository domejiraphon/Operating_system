/*
https://stackoverflow.com/questions/9966663/1-byte-unsigned-integer-c
https://www.programmingsimplified.com/c-program-read-file
https://cplusplus.com/reference/cstdio/fread/
https://stackoverflow.com/questions/48367022/c-iterate-through-char-array-with-a-pointer
https://stackoverflow.com/questions/13975760/split-files-into-fixed-size-blocks-in-c
https://gist.github.com/marcetcheverry/991042
https://www.youtube.com/watch?v=_n2hE2gyPxU&ab_channel=CodeVault
https://stackoverflow.com/a/26989434
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
//#include "utils.h"

#define CHUNK_SIZE 4096
#define SIZE_MAX 8192
#define MAX_TASKS 10000000

bool submitAllJobs = false;
pthread_mutex_t mutexQueue;
pthread_mutex_t submitMutex;
pthread_cond_t emptyQueue;

struct Task {
  int id;
  int num;
  char *content;
};
struct Task *taskQueue[MAX_TASKS];
int numJobs=0;
char *results[MAX_TASKS];
int resultsLength[MAX_TASKS];
int head=0;
int taskId=0;

void submitJobs(int fd, int i, size_t length, int offset){
  char *content = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, offset * CHUNK_SIZE);
  if (content == MAP_FAILED)
    printf("mmap failed");
  
  struct Task *currentJobs = (struct Task *)malloc(sizeof(struct Task));
  currentJobs -> id = i;
  
  currentJobs -> content = content;
  currentJobs -> num = length;
  taskQueue[i] = currentJobs;
}

void readFileAndSplit(int argc, char **argv, bool foundOpt){
  int fd;
  struct stat sb;
  
  for (int i=(foundOpt) ? 3 : 1; i<argc && strcmp(argv[i], ">"); i++){
    fd = open(argv[i], O_RDONLY);
    if (fd == -1)
      printf("Error openning the file");
    if (fstat(fd, &sb) == -1)
      printf("Error getting file information");
    int totalFileSize = sb.st_size / sizeof(char);

    int offset=0;
    while (totalFileSize > 0){
      pthread_mutex_lock(&mutexQueue);
      submitJobs(fd, taskId++, 
        (totalFileSize < CHUNK_SIZE) ? totalFileSize : CHUNK_SIZE, offset++);
      totalFileSize -= CHUNK_SIZE;
      numJobs++;
      
      pthread_mutex_unlock(&mutexQueue);
      pthread_cond_signal(&emptyQueue);
    }
    close(fd);
  }
  submitAllJobs = true;
}

void runLenthEncoding(struct Task *task){
  int rep=0;
  char *content = task -> content;
  int n = task -> num;
  char *out = (char *)(malloc(sizeof(char *) * SIZE_MAX));
  int idx = 0;
  for (int i=0; i<= n; i++){
    if ((i == n) || (i != 0 && content[i] != content[i-1])){
      //printf("%c %d\n", content[i - 1] + '\0', rep);
      out[idx++] = content[i-1];
      out[idx++] = rep;
      rep = 1;
    }
    else
      rep++;
  }
  
  results[task -> id] = out;
  resultsLength[task -> id] = idx;
  //printf("%d: %s", task -> id, results[task -> id]);
  //fflush(stdout);
}

void *startThread(void *args){
  while (1){
    pthread_mutex_lock(&mutexQueue);
    while (numJobs == 0 && !submitAllJobs)
      pthread_cond_wait(&emptyQueue, &mutexQueue);
   
    if (numJobs == 0){
      pthread_mutex_unlock(&mutexQueue);
      break;
    }
    
    numJobs--;
    struct Task *cur = taskQueue[head++];
    pthread_mutex_unlock(&mutexQueue);
    runLenthEncoding(cur);
    free(cur);
  }
  return NULL;
}

void merge(int i){
  char *first = results[i];
  char *second = results[i+1];
  int idx=0;
  /*
  while (results[0][idx]){
    printf("%c %d\n", results[0][idx], results[0][idx + 1]);
    idx  += 2;
  }
  exit(0);
  */
  
  if (results[i][resultsLength[i] - 2] == results[i+1][0]){
    results[i+1][1] += results[i][resultsLength[i] - 1];
    resultsLength[i] -= 2;
  }
    
}

void combineResults(){
  int i=0;
  while (results[i + 1]){
    merge(i++);
  }
}

void encoder(int argc, int numThreads, char **argv, bool foundOpt){
  pthread_t thread[numThreads];
  pthread_mutex_init(&mutexQueue, NULL);
  pthread_mutex_init(&submitMutex, NULL);
  pthread_cond_init(&emptyQueue, NULL);
  
  for (int i=0; i<numThreads; i++){
    if (pthread_create(&thread[i], NULL, &startThread, NULL))
      printf("Failed to create a thread");
  }
  
  readFileAndSplit(argc, argv, foundOpt);
  
  for (int i = 0; i<numThreads; i++) {
    if (pthread_join(thread[i], NULL) != 0) {
      perror("Failed to join the thread");
    }
  }
  
  combineResults();
  for (int i=0; i < taskId; i++){
    write(1, results[i], resultsLength[i]);
    free(results[i]);
  }
    
    //write(1, results[i], resultsLength[i]);
    
  

  pthread_mutex_destroy(&mutexQueue);
  pthread_mutex_destroy(&submitMutex);
  pthread_cond_destroy(&emptyQueue);
}

int main(int argc, char **argv) {
  int numThreads = 1, opt;
  bool foundOpt=false;
  while ((opt = getopt(argc, argv, "j:")) != -1) {
    switch (opt) {
    case 'j':
      numThreads = *optarg - '0';
      foundOpt = true;
      break;
    default:
      fprintf(stderr, "Usage: ./nyuenc [-j numThreads] file ...\n");
      exit(EXIT_FAILURE);
    }
  }
  
  encoder(argc, numThreads, argv, foundOpt);
  return 0;
}