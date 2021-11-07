/* 
 * coarse grain lock
 * strike a balance between code complexity and performance
 */

#include <pthread.h>
#include <stdlib.h>

#define NHASH 29
#define HASH(id) (((unsigned long)id) % NHASH)

struct foo *fh[NHASH];

pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;

struct foo {
  int f_count;
  pthread_mutex_t f_lock;
  int f_id;
  struct foo *f_next; /* protected by hashlock */
  /*... more stuff here ... */
};

/* allocate the object */
foo *foo_alloc(int id) {
  foo *fp;
  int idx;

  if ((fp = (foo *)malloc(sizeof(foo))) != nullptr) {
    fp->f_count = 1;
    fp->f_id = id;
    if (pthread_mutex_init(&fp->f_lock, nullptr) != 0) {
      free(fp);
      return nullptr;
    }
    idx = HASH(id);

    pthread_mutex_lock(&hashlock);
    fp->f_next = fh[idx];
    fh[idx] = fp;
    pthread_mutex_lock(&fp->f_lock);
    pthread_mutex_unlock(&hashlock);

    /* ... continue initialization ...*/
    pthread_mutex_unlock(&fp->f_lock);
  }
  return fp;
}

/* add a reference to the object */
void foo_hold(foo *fp) {
  pthread_mutex_lock(&fp->f_lock);
  fp->f_count++;
  pthread_mutex_unlock(&fp->f_lock);
}

/* find an existing object */
foo *foo_find(int id) {
  foo *fp;
  pthread_mutex_lock(&hashlock);
  for (fp = fh[HASH(id)]; fp != NULL; fp = fp->f_next) {
    if (fp->f_id == id) {
      foo_hold(fp);
      break;
    }
  }
  pthread_mutex_unlock(&hashlock);
  return fp;
}

/* release a reference to the object */
void foo_rele(foo *fp) {
  foo *tfp;
  int idx;

  pthread_mutex_lock(&hashlock);
  if (--fp->f_count == 0) { /* last reference*/
    /* remove from the list */
    idx = HASH(fp->f_id);
    tfp = fh[idx];
    if (tfp == fp) {
      fh[idx] = fp->f_next;
    } else {
      while (tfp->f_next != fp) {
        tfp = tfp->f_next;
      }
      tfp->f_next = fp->f_next;
    }

    pthread_mutex_unlock(&hashlock);
    pthread_mutex_destroy(&fp->f_lock);
    free(fp);
  } else {
    pthread_mutex_unlock(&hashlock);
  }
}
