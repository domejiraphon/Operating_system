#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "utils.h" 

#define THREAD_NUM 10

Task taskQueue[10000000];
pthread_mutex_t mutexQueue;
pthread_cond_t emptyQueue;
int count=0;

void execute(Task* task){
  int result = task -> a + task -> b;
  printf("%d: %d\n", task -> id, result);
}

void *startThread(void *args){
  while (1){
    pthread_mutex_lock(&mutexQueue);
    while (count == 0)
      pthread_cond_wait(&emptyQueue, &mutexQueue);
    for (int i=0; i<count - 1; i++)
      taskQueue[i] = taskQueue[i + 1];
    count--;
    Task cur = taskQueue[0];
    pthread_mutex_unlock(&mutexQueue);
    execute(&cur);
  }
}

void submitTask(Task* task){
  pthread_mutex_lock(&mutexQueue);
  taskQueue[count++] = *task;
  pthread_mutex_unlock(&mutexQueue);
  pthread_cond_signal(&emptyQueue);
}

int main(){
  pthread_t thread[THREAD_NUM];
  pthread_mutex_init(&mutexQueue, NULL);
    pthread_cond_init(&emptyQueue, NULL);
  for (int i=0; i<THREAD_NUM; i++){
    if (pthread_create(&thread[i], NULL, &startThread, NULL))
      printf("Failed to create a thread");
  }
  for (int i=0; i<1000; i++){
    Task* cur = (Task *)malloc(sizeof(Task *));
    cur -> id = i;
    cur -> a = rand() % 100;
    cur -> b =rand() % 100;
    submitTask(cur);
  }
  for (int i = 0; i < THREAD_NUM; i++) {
    if (pthread_join(thread[i], NULL) != 0) {
      perror("Failed to join the thread");
    }
  }
  pthread_mutex_destroy(&mutexQueue);
  pthread_cond_destroy(&emptyQueue);
}