#include "../apue.h"
#include <pthread.h>

void *thr_fn1(void *args) {
  printf("thread 1 returning\n");
  return ((void *)1);
}

void *thr_fn2(void *args) {
  printf("thread 2 returning\n");
  return ((void *)2);
}

int main() {
  pthread_t tid1, tid2;
  void *ret;
  int err = pthread_create(&tid1, nullptr, thr_fn1, nullptr);
  if (err != 0) {
    printf("Error, can't create thread1\n");
    return 1;
  }
  err = pthread_create(&tid2, nullptr, thr_fn2, nullptr);
  if (err != 0) {
    printf("Error, can't create thread2\n");
    return 1;
  }
  err = pthread_join(tid1, &ret);
  if (err != 0) {
    printf("can't create thead1");
    return 1;
  }
  printf("thread 1 exit code %ld\n", (long)ret);
  err = pthread_join(tid2, &ret);
  if (err != 0) {
    printf("can't create thead2");
    return 1;
  }
  printf("thread 2 exit code %ld\n", (long)ret);
  return 0;
}
