#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h> 

#define N 20000

#define NTHREADS 2

void populatedata(double * x, double * y);
void scientistfunction(int i, double * t, double * H, double * k, double test, int sizeData);

int main(int argc, char **argv) {
  
  omp_set_num_threads(NTHREADS);

  double * t=(double *)malloc(sizeof(double)*N);
  double * H=(double *)malloc(sizeof(double)*N);
  double * k=(double *)malloc(sizeof(double)*N);
  printf("\nAllocated %f GiB", (sizeof(double)*N*3.0)/(1024*1024*1024.0));
  populatedata(t, H);

  

  
  
  int x;
  double test;
  //Parallelize here
  #pragma omp parallel shared(test,t,H,k) private(x)
  {
  	#pragma omp for
  	for (x=0; x<N; x++)
  	{
    	test=0.000001+(x*0.0001);
    	scientistfunction(x, t, H, k, test, N); 
  	}
  } 


  //For validation
  double sum=0;
  for (x=0; x<N; x++)
  {
    sum+=k[x];
  }  
  printf("\nSum after: %f",sum);


  free(t);
  free(H);
  free(k);
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



void scientistfunction(int i, double * t, double * H, double * k, double test, int sizeData)
{ 
        double c, s, xc, xs, cc, ss, cs;
        double tau, c_tau, s_tau, c_tau2, s_tau2, cs_tau;
          xc = 0.0;
          xs = 0.0;
          cc = 0.0;
          ss = 0.0;
          cs = 0.0;

          for (unsigned int j=0; j<sizeData; j++)
          { 
              c = cos(test * t[j]);
              s = sin(test * t[j]);

              xc += H[j] * c;
              xs += H[j] * s;
              cc += c * c;
              ss += s * s;
              cs += c * s;
          }
              
          tau = atan2(2.0 * cs, cc - ss) / (2.0 * test);
          c_tau = cos(test * tau);
          s_tau = sin(test * tau);
          c_tau2 = c_tau * c_tau;
          s_tau2 = s_tau * s_tau;
          cs_tau = 2.0 * c_tau * s_tau;

          k[i] = 0.5 * ((((c_tau * xc + s_tau * xs)*(c_tau * xc + s_tau * xs)) / 
              (c_tau2 * cc + cs_tau * cs + s_tau2 * ss)) + 
              (((c_tau * xs - s_tau * xc)*(c_tau * xs - s_tau * xc)) / 
              (c_tau2 * ss - cs_tau * cs + s_tau2 * cc)));
}


