/*
https://stackoverflow.com/questions/9966663/1-byte-unsigned-integer-c
https://www.programmingsimplified.com/c-program-read-file
https://cplusplus.com/reference/cstdio/fread/
https://stackoverflow.com/questions/48367022/c-iterate-through-char-array-with-a-pointer
https://stackoverflow.com/questions/13975760/split-files-into-fixed-size-blocks-in-c
https://gist.github.com/marcetcheverry/991042
https://www.youtube.com/watch?v=_n2hE2gyPxU&ab_channel=CodeVault
https://stackoverflow.com/a/26989434
https://stackoverflow.com/questions/6979892/how-to-free-memory-allocated-using-mmap
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

#define CHUNK_SIZE 4096
#define SIZE_MAX 50
#define MAX_TASKS 250000

bool submitAllJobs = false;
pthread_mutex_t mutexQueue;
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

void submitJobs(char *content, int i, size_t length, int offset){
  /*
  create each task in the task queue
  */
  struct Task *currentJobs = (struct Task *)malloc(sizeof(struct Task));
  currentJobs -> id = i;
  currentJobs -> content = content + offset * CHUNK_SIZE;
  currentJobs -> num = length;
  taskQueue[i] = currentJobs;
}

void readFileAndSplit(int argc, char **argv, bool foundOpt){
  /*
    split each file into each task where the chunk size is = 4096
  */
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

    //read file
    char *content = mmap(NULL, totalFileSize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (content == MAP_FAILED)
      printf("mmap failed");
    
    //submit jobs to the job queue
    while (totalFileSize > 0){
      submitJobs(content, taskId++, 
        (totalFileSize < CHUNK_SIZE) ? totalFileSize : CHUNK_SIZE, offset++);
      totalFileSize -= CHUNK_SIZE;
      pthread_mutex_lock(&mutexQueue);
      numJobs++;
      pthread_cond_signal(&emptyQueue);
      pthread_mutex_unlock(&mutexQueue);
      
    }
    close(fd);
  }
  submitAllJobs = true;

}

void runLenthEncoding(struct Task *task){
  /*
    Algorithm to encode the file
  */
  int rep=0;
  char *content = task -> content;
  int n = task -> num;
  char *out = (char *)(malloc(sizeof(char *) * SIZE_MAX));
  int idx = 0;
  for (int i=0; i<= n; i++){
    if ((i == n) || (i != 0 && content[i] != content[i-1])){
      out[idx++] = content[i-1];
      out[idx++] = rep;
      rep = 1;
    }
    else
      rep++;
  }
  
  //write the results in the array
  results[task -> id] = out;
  resultsLength[task -> id] = idx;
}

void *startThread(){
  /*
  each thread perform this task
  */
  while (1){
    pthread_mutex_lock(&mutexQueue);
    //wait until there is a job
    while (numJobs == 0 && !submitAllJobs)
      pthread_cond_wait(&emptyQueue, &mutexQueue);

    if (numJobs == 0){
      pthread_mutex_unlock(&mutexQueue);
      break;
    }
    
    numJobs--;
    //get task
    struct Task *cur = taskQueue[head++];
    pthread_mutex_unlock(&mutexQueue);
    runLenthEncoding(cur);
    free(cur);
  }
  return NULL;
}

void merge(int i){
  //merge result only in the boundary area
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
  pthread_cond_init(&emptyQueue, NULL);
  
  //read and create a task queue
  readFileAndSplit(argc, argv, foundOpt);

  //create threads
  for (int i=0; i<numThreads; i++){
    if (pthread_create(&thread[i], NULL, &startThread, NULL))
      printf("Failed to create a thread");
  }
  
  //wait for all threads to finish
  for (int i = 0; i<numThreads; i++) {
    if (pthread_join(thread[i], NULL))
      perror("Failed to join the thread");
  }

  //combine results
  combineResults();

  //write the results out to stdout
  for (int i=0; i < taskId; i++){
    write(1, results[i], resultsLength[i]);
    free(results[i]);
  }

  pthread_mutex_destroy(&mutexQueue);
  pthread_cond_destroy(&emptyQueue);
}

int main(int argc, char **argv) {
  int numThreads = 1, opt;
  bool foundOpt=false;

  //get the job option
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