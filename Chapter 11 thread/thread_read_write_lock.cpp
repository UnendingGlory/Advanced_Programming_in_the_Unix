/* more efficient when writing lock is more often than reading lock */

#include <stdlib.h>
#include <pthread.h>

struct job {
  struct job *j_next;
  struct job *j_prev;
  pthread_t j_id; /* tells which thread handles this job */
  /* ... more stuff here ... */
};

/* job dequeue, protected by a single read-wite lock */
struct queue {
  struct job *q_head;
  struct job *q_tail;
  pthread_rwlock_t q_lock;
};

/*
 * Initialize a queue.
 */
int queue_init(queue *qp) {
  int err;
  qp->q_head = nullptr;
  qp->q_tail = nullptr;
  err = pthread_rwlock_init(&qp->q_lock, nullptr);
  if (err != 0) { return err; }
  /* ... continue initializtion ... */
  return 0;
}

/*
 * Insert a job at the head of the queue.
 * Insert to the head
 */
void job_insert(queue *qp, job *jp) {
  pthread_rwlock_wrlock(&qp->q_lock); // lock write
  jp->j_next = qp->q_head;
  jp->j_prev = nullptr;

  /* change the queue head and tail */
  if (qp->q_head != nullptr) {
    qp->q_head->j_prev = jp;
  } else { /* list was empty */
    qp->q_tail = jp;
  }

  qp->q_head = jp;
  pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Append a job on the tail of the queue.
 */
void job_append(queue *qp, job *jp) {
  pthread_rwlock_wrlock(&qp->q_lock);
  jp->j_next == nullptr;
  jp->j_prev = qp->q_tail;

  if (qp->q_tail != nullptr) {
    qp->q_tail->j_next = jp;
  } else {
    qp->q_head = jp; /* list was empty */
  }
  qp->q_tail = jp;

  pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Remove the given job from a queue
 */
void job_remove(queue *qp, job *jp) {
  pthread_rwlock_wrlock(&qp->q_lock);
  if (jp == qp->q_head) { // remove from head
    qp->q_head = jp->j_next;
    if (qp->q_tail == jp) qp->q_tail = nullptr; // the last node
    else {
      jp->j_next->j_prev = jp->j_prev;
    }
  } else if (jp == qp->q_tail) {
      qp->q_tail = jp->j_prev;
      jp->j_next->j_prev = jp->j_prev;
  } else {
      jp->j_prev->j_next = jp->j_next;
      jp->j_next->j_prev = jp->j_prev;
  }
  pthread_rwlock_unlock(&qp->q_lock);
}

/*
 * Find a job for the given thread ID
 */
job *job_find(queue *qp, pthread_t id) {
  job *jp;

  if (pthread_rwlock_rdlock(&qp->q_lock) != 0) {
    return nullptr;
  }

  for (jp = qp->q_head; jp != nullptr; jp = jp->j_next) {
    if (pthread_equal(jp->j_id, id))
      break;
  }

  pthread_rwlock_unlock(&qp->q_lock);
  return jp;
}

