#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define N 10000000

#define NTHREADS 2

void populatedata(double * x, double * y);
void scientistfunction(int i, double * t, double * H, double * k, double test, int sizeData);


int main(int argc, char **argv) {

  srand(17);

  omp_set_num_threads(NTHREADS);

  double * a=(double *)malloc(sizeof(double)*N);
  double * b=(double *)malloc(sizeof(double)*N);
  populatedata(a, b);
  printf("\nAllocated %f GiB", (sizeof(double)*N*2.0)/(1024*1024*1024.0));



  int x;
  double sum=0;

  double tstart=omp_get_wtime();

  for (x=0; x<N; x++)
  {
    sum+=a[x]+b[x];
  }

  double tend=omp_get_wtime();
  printf("\nSum: %f",sum);
  printf("\nTime sequential: %f", tend - tstart);

  sum=0;

  tstart=omp_get_wtime();

  //Parallelize with critical section
  for (x=0; x<N; x++)
  {
      #pragma omp critical
      {
          sum+=a[x]+b[x];
      }
  }

  tend=omp_get_wtime();
  printf("\nSum: %f",sum);
  printf("\nTime critical section: %f", tend - tstart);


  sum=0;

  tstart=omp_get_wtime();

  //Parallelize with atomic section
  for (x=0; x<N; x++)
  {
    #pragma omp atomic
    sum+=a[x]+b[x];
  }

  tend=omp_get_wtime();
  printf("\nSum: %f",sum);
  printf("\nTime atomic section: %f", tend - tstart);



  free(a);
  free(b);
  printf("\n\n");
  return 0;
}

void populatedata(double * x, double * y)
{
  int i;
  for (i=0; i<N; i++)
  {
    x[i]=i;
    y[i]=i;
  }

}
