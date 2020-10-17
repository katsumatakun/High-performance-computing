#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define NB_THREADS 10
pthread_barrier_t barrier;
void *job(void *_i) {
  int i = *((int *) _i);
  printf("thread: %d\n", i);
}

int main () {
  int ints[NB_THREADS];
  pthread_t    threads[NB_THREADS];
  for (int i = 0; i < NB_THREADS; ++i) {
    ints[i] = i;
    pthread_create(&threads[i], NULL, job, &ints[i]);
  }
}
