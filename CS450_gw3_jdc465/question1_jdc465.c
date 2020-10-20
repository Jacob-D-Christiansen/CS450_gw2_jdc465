#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h> 

#define N 20000000

void populatedata(int * x, int * y);

int main(int argc, char **argv) {
  
  omp_set_num_threads(2);

  int * x=(int *)malloc(sizeof(int)*N);
  int * y=(int *)malloc(sizeof(int)*N);
  int * z=(int *)malloc(sizeof(int)*N);
  printf("\nAllocated %f GiB", (sizeof(int)*N*3.0)/(1024*1024*1024.0));
  populatedata(x, y);

  int i;
  unsigned int sum;

  //Rewrite the OpenMP code

  //#pragma omp parallel sections private(i) shared(x, y, z)
  //{

  for (i=0; i<N; i++)
  {
    #pragma omp sections
  {
     #pragma omp section
     {
     z[i]=x[i]+y[i];
     }
  }
  }
  //}
    /*
    #pragma omp section
    {
      for (i=N/2; i<N; i++)
      {
        z[i]=x[i]+y[i];
      } 
    }*/
  


  //For validation
  sum=0;
  for (int i=0; i<N; i++)
  {
    sum+=z[i];
  }  
  printf("\nSum after: %d",sum);


  free(x);
  free(y);
  printf("\n\n");
  return 0;
}

void populatedata(int * x, int * y)
{
  int sum=0;
  int i;
  for (i=0; i<N; i++)
  {
    x[i]=i;
    y[i]=i;
    sum+=x[i]+y[i];
  }
  //For validation
  printf("\nSum before: %d",sum);

}
