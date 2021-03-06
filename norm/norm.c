#include <cilk/cilk.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

float tdiff(struct timeval *start, struct timeval *end) {
  return (end->tv_sec-start->tv_sec) + 1e-6*(end->tv_usec-start->tv_usec);
}

__attribute__((const,noinline))
double mag(const double *A, int n) {
  double amt = 0;
  for(int i=0; i<n; i++) amt+= A[i];
  return amt;
}

__attribute__((noinline))
void serial_normalize(double *restrict out, const double *restrict in, int n) {
 for(int i = 0; i < n; ++i)
    out[i] = in[i] / mag(in, n);
}

__attribute__((noinline))
void parallel_normalize(double *restrict out, const double *restrict in, int n) {
 cilk_for(int i = 0; i < n; ++i)
    out[i] = in[i] / mag(in, n);
}

int main(int argc, char *argv[]) {
  struct timeval start, end;
  int n = 1000000;
  int x = 20;
  if (argc > 1) {
    n = atoi(argv[1]);
    if (argc > 2) {
      x = atoi(argv[2]);
    }
  }

  double *A = (double*)malloc(sizeof(double) * n);
  assert(A != 0);

  double *B = (double*)malloc(sizeof(double) * n);
  assert(B != 0);
  for(int i=0; i<n; i++)
   B[i] = x; 


  gettimeofday(&start, NULL);

  serial_normalize(A, B, n);

  gettimeofday(&end, NULL);
  printf("Serial Normalize %0.6f %f\n", tdiff(&start, &end), A[n-1]);

  gettimeofday(&start, NULL);

  parallel_normalize(A, B, n);

  gettimeofday(&end, NULL);
  printf("Parallel Normalize %0.6f %f\n", tdiff(&start, &end), A[n-1]);

  free(A);
  free(B);
  return 0;
}
