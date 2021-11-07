#include <stdlib.h>
#include <pthread.h>

struct foo {
  int f_count;
  pthread_mutex_t f_lock;
  int f_id;
  /* ... more stuff here ... */
};

foo *foo_alloc(int id) {
  foo *fp;
  if ((fp = (foo *)malloc(sizeof(foo))) != nullptr) {
    fp->f_id = 1;
    fp->f_id = id;
    if (pthread_mutex_init(&fp->f_lock, nullptr) != 0) {
      free(fp);
      return nullptr;
    }
    /* ... continue initialization ... */
  }
  return fp;
}

/* add a reference to the object */
void foo_hold(foo *fp) {
  pthread_mutex_lock(&fp->f_lock);
  fp->f_count++;
  pthread_mutex_unlock(&fp->f_lock);
}

/* release a reference to the object */
void foo_rele(foo *fp) {
  pthread_mutex_lock(&fp->f_lock);
  if (--fp->f_count == 0) { // last reference
    pthread_mutex_unlock(&fp->f_lock);
    pthread_mutex_destroy(&fp->f_lock);
    free(fp);
  } else {
    pthread_mutex_unlock(&fp->f_lock);
  }
}
