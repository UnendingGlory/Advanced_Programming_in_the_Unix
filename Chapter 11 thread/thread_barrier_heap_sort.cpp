/** run the script: g++ -pthread thread_heap_sort.cpp -o test */
#include "../apue.h"
#include <limits.h>
#include <pthread.h>
#include <sys/time.h>

#define NTHR 8               /* number of threads */
#define NUMNUM 8000000L      /* number of numbers to sort */
#define TNUM (NUMNUM / NTHR) /* number to sort per thread */

long nums[NUMNUM];
long snums[NUMNUM];  /* store sorted result */

pthread_barrier_t b; /* thread barrier */

#ifdef SOLARIS
#define heapsort qsort
#else
int heapsort(void *, size_t, size_t, int (*)(const void *, const void *));
#endif

/*
 * Compare two long integers (helper function for heapsort)
 */
int complong(const void *arg1, const void *arg2) {
  long l1 = *(long *)arg1;
  long l2 = *(long *)arg2;

  if (l1 == l2) {
    return 0;
  } else if (l1 < l2) {
    return -1;
  } else {
    return 1;
  }
}

/*
 * Helper function of the heapsort
 * down ajust the ind-th num in the heap
 */
void down_adjust(void *arr, size_t ind, size_t n) {
  // printf("Down adjust start\n");
  long *heap = (long *)arr;
  long temp;
  size_t i = ind, j = 2 * ind + 1;
  while (j < n) {
    if ((j + 1 < n) && (heap[j + 1] > heap[j])) { // right child larger
      ++j;
    }
    if (heap[j] > heap[i]) {
      temp = heap[j];
      heap[j] = heap[i];
      heap[i] = temp;
      
      i = j;
      j = 2 * i + 1;
    }
    else { break; } // adjust over
  }
}

/*
 * Implement the heapsort algo, max heap(acsending)
 */
int heapsort(void *arr, size_t n, size_t ele_size,
             int (*comp_func)(const void *, const void *)) {
  long *start = (long *)arr;
  /* 
   * array is a complete binary tree
   * if node idx: i, left child idx: 2*i + 1, right child idx: 2*i + 2
   * idx child's father: (idx - 1) / 2
   * 
   * step 1: build max heap, start from the last non-leaf node
   * step 2: heapsort, swap the first num with the last num
   *         then down_adjust the rest arr
   */
  long *heap = (long *)arr;
  long temp, i;

  for (i = (n / 2 - 1); i >= 0; --i) {
    down_adjust(arr, i, n);
  }

  for (i = n - 1; i >= 0; --i) {
    temp = heap[i];
    heap[i] = heap[0];
    heap[0] = temp;
    
    down_adjust(arr, 0, i);
  }
  
}

/*
 * Worker thread to sort a portion of the set of numbers.
 */
void *thr_fn(void *arg) {
  long idx = (long)arg;
  heapsort(&nums[idx], TNUM, sizeof(long), complong);
  /* add a count to the thread barrier
   * if number is adequate, continue to work, else the thread sleeps
   */
  pthread_barrier_wait(&b); 
  /*
   * Go off and perform more work ...
   */
  return ((void *)0);
}

/*
 * Merge the results of the individual sorted ranges.
 */
void merge() {
  long idx[NTHR];
  long i, sidx, minidx, num;
  for (i = 0; i < NTHR; i++)
    idx[i] = i * TNUM; // start idx of the num of each thread to process
  for (sidx = 0; sidx < NUMNUM; ++sidx) {
    num = LONG_MAX;
    for (i = 0; i < NTHR; ++i) { // 8 blocks controlled by 8 threads
      // if start idx of this thread not exceed this thread's control
      if ((idx[i] < (i + 1) * TNUM) && (nums[idx[i]] < num)) {
        num = nums[idx[i]];
        minidx = i;
      }
    }
    snums[sidx] = nums[idx[minidx]]; // find the minumum number of each threads 
    idx[minidx]++;                   // start idx of the selected thread incre
  }
}

/*
 * Single thread to test the time elapse
 */
void single_thread_heapsort() {
  unsigned long i;
  struct timeval start, end;
  long long startusec, endusec;
  double elapsed; // time elapsed

  srandom(1);
  for (i = 0; i < NUMNUM; i++) {
    nums[i] = random();
  }

  gettimeofday(&start, nullptr);

  printf("Start single thread heap sort\n");
  heapsort(nums, NUMNUM, sizeof(long), complong);
  
  gettimeofday(&end, nullptr);

  startusec = start.tv_sec * 1000000 + start.tv_usec; // seconds and nanoseconds
  endusec = end.tv_sec * 1000000 + end.tv_usec;
  elapsed = (double)(endusec - startusec) / 1000000.0; // convert to seconds
  printf("Single Thread sort took %.4f seconds\n", elapsed);
  for (int i = 0; i < 20; ++i) {
    printf("%ld ", nums[i]);
  }
}

/*
 * Multiple threads to test the time elapse
 */
void multi_thread_heapsort() {
  unsigned long i;
  struct timeval start, end;
  long long startusec, endusec;
  double elapsed; // time elapsed
  int err;
  pthread_t tid;

  /*
   * Create the initial set of numbers to sort.
   */
  srandom(1);
  for (i = 0; i < NUMNUM; i++) {
    nums[i] = random();
  }

  printf("Start multi thread heap sort\n");
  /*
   * Create 8 threads to sort the numbers
   */
  gettimeofday(&start, nullptr);
  pthread_barrier_init(&b, nullptr, NTHR + 1); // include the main threads
  for (i = 0; i < NTHR; ++i) {
    err = pthread_create(&tid, nullptr, thr_fn,
                         (void *)(i * TNUM)); //  arg for thr_fn(containing heapsort)
    if (err != 0) {
      err_exit(err, "can not create the thread");
    }
  }

  pthread_barrier_wait(&b); // main thread runs to here
  merge();
  gettimeofday(&end, nullptr);

  /*
   * Print the sorted list.
   */
  startusec = start.tv_sec * 1000000 + start.tv_usec;
  endusec = end.tv_sec * 1000000 + end.tv_usec;
  elapsed = (double)(endusec - startusec) / 1000000.0; // convert to seconds
  printf("Multithread sort took %.4f seconds\n", elapsed);
  for (int i = 0; i < 20; ++i) {
    printf("%ld ", snums[i]);
    // static_assert()
  }
}

int main() {
  single_thread_heapsort();
  printf("\n");
  multi_thread_heapsort();
  return 0;
}
