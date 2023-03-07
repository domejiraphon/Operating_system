#include <pthread.h>
#include <stdio.h>

#define NUM_THREADS 2

int counter = 0;
pthread_spinlock_t lock;

void *thread_func(void *threadid) {
  long tid;
  tid = (long)threadid;
  printf("Thread %ld starting...\n", tid);
  
  // acquire the spinlock
  pthread_spin_lock(&lock);
  
  // simulate a long-running task
  sleep(5);
  
  // increment the counter
  counter++;
  
  // release the spinlock
  pthread_spin_unlock(&lock);
  
  printf("Thread %ld exiting...\n", tid);
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  pthread_t threads[NUM_THREADS];
  int rc;
  long t;
  
  // initialize the spinlock
  pthread_spin_init(&lock, 0);

  // create the threads
  for (t = 0; t < NUM_THREADS; t++) {
    rc = pthread_create(&threads[t], NULL, thread_func, (void *)t);
    if (rc) {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      return -1;
    }
  }

  // join the threads
  for (t = 0; t < NUM_THREADS; t++) {
    rc = pthread_join(threads[t], NULL);
    if (rc) {
      printf("ERROR; return code from pthread_join() is %d\n", rc);
      return -1;
    }
  }

  // destroy the spinlock
  pthread_spin_destroy(&lock);

  printf("Final counter value: %d\n", counter);

  return 0;
}
