/**
 *  pthread_exit传回的空指针指向的内存必须是有效的
 */

#include "../apue.h"
#include <pthread.h>

struct foo {
  int a, b, c, d;
};

void printfoo(const char *s, const struct foo *fp) {
  printf("%s", s);
  printf("  structure at 0x%lx\n", (unsigned long)fp);
  printf("  foo.a = %d\n", fp->a);
  printf("  foo.b = %d\n", fp->b);
  printf("  foo.c = %d\n", fp->c);
  printf("  foo.d = %d\n", fp->d);
}

void *thr_fn1(void *args) {
  struct foo foo = {1, 2, 3, 4};
  printfoo("thread 1: \n", &foo);
  pthread_exit((void *)&foo);
}

void *thr_fn2(void *args) {
  printf("thread 2: ID is %lu\n", (unsigned long)pthread_self());
  pthread_exit((void *)0);
}

int main() {
  int err;
  pthread_t tid1, tid2;
  struct foo *fp;
  err = pthread_create(&tid1, nullptr, thr_fn1, nullptr);
  if (err != 0) {
    printf("Error, can't create thread1\n");
    return 1;
  }
  err = pthread_join(tid1, (void **)&fp);
  if (err != 0) {
    printf("Error, can't join thread1\n");
    return 1;
  }
  sleep(1); // wait for the thread stack to recycle
  printf("parent starting second thread\n");
  err = pthread_create(&tid2, nullptr, thr_fn2, nullptr);
  if (err != 0) {
    printf("Error, can't create thread2\n");
    return 1;
  }
  sleep(1);
  printfoo("parent:\n", fp);

  return 0;
}
