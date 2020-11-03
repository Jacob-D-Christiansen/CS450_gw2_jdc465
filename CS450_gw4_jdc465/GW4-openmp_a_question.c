#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h> 

#define N 40000
#define THRESHOLD 1.0

#define NTHREADS 1

void populatedata(double * x, double * y);
double expodist(double lambda);
double computedistance(double * a, double * b);
int main(int argc, char **argv) {
  
  //do not change
  srand(26);

  omp_set_num_threads(NTHREADS);

  double * x=(double *)malloc(sizeof(double)*N);
  double * y=(double *)malloc(sizeof(double)*N);
  int * z=(int *)malloc(sizeof(int)*N);
  double runningSum = 0.0;
  printf("\nAllocated %f GiB\n", ((sizeof(double)*N*2.0)+(sizeof(int)*N*1.0))/(1024*1024*1024.0));
  populatedata(x, y);

  //Write code here
  double tstart = omp_get_wtime();
  
  #pragma omp parrallel shared(z, x, y, N) private(i)
  for(int i = 0; i < N; i++)
  {
    z[i] = (int)computedistance(&x[i], &y[i]);
  }
  
  double tend = omp_get_wtime();
  
  for(int i = 0; i < N; i++)
  {
    runningSum += z[i];
  }


  free(x);
  free(y);
  free(z);
  printf("Total Time: %f\n", tend-tstart);
  printf("z sum: %f\n", runningSum);
  printf("\n\n");
  return 0;
}

double computedistance(double * a, double * b)
{
  return(fabs((*a)-(*b)));
}

void populatedata(double * x, double * y)
{
  double lambda=0.5;
  int i;
  for (i=0; i<N; i++)
  {
    x[i]=expodist(lambda);
    y[i]=expodist(lambda);
    
  }
}

double expodist(double lambda)
{
    double u;
    u = rand() / (RAND_MAX + 1.0);
    return -log(1- u) / lambda;
}
