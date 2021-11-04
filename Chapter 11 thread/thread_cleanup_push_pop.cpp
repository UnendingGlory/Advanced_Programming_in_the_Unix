#include "../apue.h"
#include <pthread.h>

void cleanup(void *arg) { printf("clean up: %s\n", (char *)arg); }

void *thr_fn1(void *arg) {
  printf("thread 1 starts\n");
  pthread_cleanup_push(
      cleanup, (void *)"thread 1 first handler"); // const char * to void *
  pthread_cleanup_push(cleanup, (void *)"thread 1 second handler");
  printf("thread 1 push complete\n");
  if (arg) {
    return ((void *)1);
  }
  pthread_cleanup_pop(0); // do not use the second cleanup function
  pthread_cleanup_pop(0); // do not use the first cleanup function
  return (void *)1;
}

void *thr_fn2(void *arg) {
  printf("thread 2 starts\n");
  pthread_cleanup_push(cleanup, (void *)"thread 2 first handler");
  pthread_cleanup_push(cleanup, (void *)"thread 2 second handler");
  printf("thread 2 push complete\n");
  if (arg) {
    pthread_exit((void *)2);
  }
  pthread_cleanup_pop(0); // do not use the second cleanup function
  pthread_cleanup_pop(0); // do not use the first cleanup function
  pthread_exit((void *)2);
}

int main() {
  int err;
  pthread_t tid1, tid2;
  void *tret;
  err = pthread_create(&tid1, nullptr, thr_fn1, (void *)1);
  err = pthread_create(&tid2, nullptr, thr_fn2, (void *)1);
  err = pthread_join(tid1, &tret);
  err = pthread_join(tid2, &tret);
  return 0;
}
