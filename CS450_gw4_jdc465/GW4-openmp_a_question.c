#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h> 

#define N 40000
#define THRESHOLD 1.0

#define NTHREADS 4

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
  printf("\nAllocated %f GiB", ((sizeof(double)*N*2.0)+(sizeof(int)*N*1.0))/(1024*1024*1024.0));
  populatedata(x, y);

  //Write code here
  double tstart = omp_get_wtime();
  int sum = 0;
  int i, j;
  #pragma omp parallel shared(z, x, y) private(i, j) 
  {
   #pragma omp for schedule(dynamic)
   for(i = 0; i < N; i++) {
    for(j = 0; j < N; j++) {
     double result = computedistance(&x[i],&y[j]);
     if(result <= 1) {
     z[i]++;}
    }
   }
   }
   for(int i = 0; i < N; i++) {
    sum+= z[i];
   }
  double tend = omp_get_wtime();   

  free(x);
  free(y);
  free(z);
  printf("Total Time: %f\n", tend - tstart);
  printf("z sum: %d\n", sum);
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
