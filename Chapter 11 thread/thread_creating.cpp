#include "../apue.h"
#include <pthread.h>

pthread_t ntid;

void printids(const char *s) {
  pid_t pid;
  pthread_t tid;
  pid = getpid();
  tid = pthread_self(); // get tid for itself
  printf("%s pid %lu tid %lu (0x%lx)\n", s, (unsigned long)pid,
         (unsigned long)tid, (unsigned long)tid);
}

void *thr_fn(void *args) { // when thread create, point to this function
  printids("new thread: ");
  return ((void *)0);
}

int main() {
  int err = pthread_create(&ntid, nullptr, thr_fn, nullptr);
  if (err != 0) {
    printf("cann't create thread");
  }
  printids("main thread: ");
  sleep(1);
  return 0;
}
