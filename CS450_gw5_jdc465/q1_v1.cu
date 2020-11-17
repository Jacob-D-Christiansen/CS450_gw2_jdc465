#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <omp.h>

#define N 500000000

using namespace std;

void warmUpGPU();
__global__ void vectorAdd(unsigned int * A, unsigned int * B, unsigned int * C);

int main(int argc, char *argv[])
{
  double start;
  start = omp_get_wtime();

	warmUpGPU();


	unsigned int * A;
	unsigned int * B;
	unsigned int * C;
	unsigned int * C_CPU;

	A=(unsigned int *)malloc(sizeof(unsigned int)*N);
	B=(unsigned int *)malloc(sizeof(unsigned int)*N);
	C=(unsigned int *)malloc(sizeof(unsigned int)*N);
	C_CPU=(unsigned int *)malloc(sizeof(unsigned int)*N);


	printf("\nSize of A+B+C (GiB): %f",(sizeof(unsigned int)*N*3.0)/(1024.0*1024.0*1024.0));


	//init:
	int i=0;
	for (i=0; i<N; i++){
		A[i]=i;
		B[i]=i;
		C[i]=0;
		C_CPU[i]=0;
	}


	//CPU version:

	for (int i=0; i<N; i++){
		C_CPU[i]=A[i]+B[i];
	}


  printf("%s%lf\n", "\nTime elapsed: ", omp_get_wtime() - start);
	return 0;
}

__global__ void vectorAdd(unsigned int * A, unsigned int * B, unsigned int * C) {

unsigned int tid=threadIdx.x+ (blockIdx.x*blockDim.x);

if (tid>=N) {
	return;
}

C[tid]=A[tid]+B[tid];

return;
}




void warmUpGPU(){
cudaDeviceSynchronize();
return;

}
