#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "utils.h" 

#define THREAD_NUM 1
#define MAX_TASKS 10
typedef struct Task {
  int a;
  int id;
} Task;

Task taskQueue[MAX_TASKS];
pthread_mutex_t mutexQueue;
pthread_cond_t emptyQueue;
int count=0;
int expectedTasks=1;
int result[MAX_TASKS];
void execute(Task* task){
  result[task -> id] = 2 * task -> a;
  sleep(1);
  //printf("%d: %d\n", task -> id, result[task -> id]);
}

void *startThread(void *args){
  while (1){
    pthread_mutex_lock(&mutexQueue);
    while (count == 0 && expectedTasks > 0)
      pthread_cond_wait(&emptyQueue, &mutexQueue);
    if (count == 0 && expectedTasks == 0){
      pthread_mutex_unlock(&mutexQueue);
      break;
    }
    for (int i=0; i<count - 1; i++)
      taskQueue[i] = taskQueue[i + 1];
    count--;
    Task cur = taskQueue[0];
    pthread_mutex_unlock(&mutexQueue);
    execute(&cur);
  }
  return NULL;
}

void submitTask(Task* task){
  pthread_mutex_lock(&mutexQueue);
  if (count < MAX_TASKS){
    taskQueue[count++] = *task;
  }
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
  
  for (int i=0; i<MAX_TASKS; i++){
    Task t = {i, i};
    submitTask(&t);
  }
  expectedTasks=0;
  
  for (int i = 0; i < THREAD_NUM; i++) {
    if (pthread_join(thread[i], NULL) != 0) {
      perror("Failed to join the thread");
    }
  }
  pthread_mutex_destroy(&mutexQueue);
  pthread_cond_destroy(&emptyQueue);
  
}
