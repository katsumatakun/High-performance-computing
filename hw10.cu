#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
using namespace std;



__global__ void initArray( int *A) {
  int tid;
  tid = blockIdx.x * blockDim.x + threadIdx.x;
  A[tid] = tid;
}

__global__ void swapArray( int *A, int size) {

  for(int i=0; i<size/2; i++){
    int temp = A[i];
    A[i] = A[size-1-i];
    A[size-1-i] = temp;
  }
}


int main(){

  srand(time(0));
  int size=16*1024*1024;
  // int size=16;
  int* ary = new int[size];
  int* ans = new int[size];
  int num;
  for(int i=0; i<size; i++){
    num = rand()%100;
    ary[i] = num;
    ans[size-i-1] = num;
  }

  int *h_a, *d_a;
  // define thread hierarchy
  int num_blocks = 8; int num_th_per_blk = 8;
  // allocate host and device memory
  size_t memSize;
  // memSize = num_blocks * num_th_per_blk * sizeof(int);
  memSize = size*sizeof(int);
  // h_a = (int*) malloc(memSize);
  cudaMalloc( (void**) &d_a, memSize);

  cudaMemcpy( d_a, ary, memSize, cudaMemcpyHostToDevice);
  // launch kernel
  dim3 dimGrid(num_blocks);
  dim3 dimBlock(num_th_per_blk);
  // initArray<<< dimGrid, dimBlock >>>(d_a);
  swapArray<<< dimGrid, dimBlock >>>(d_a, size);
  // retrieve results
  cudaMemcpy( ary, d_a, memSize, cudaMemcpyDeviceToHost);

  for(int i=0; i<size; i++){
    if(ary[i]!=ans[i]){
      cout << ans[i] << " "<< ary[i] << endl;
      cout << "not match" << endl;
      break;
  }
  
}

  return 0;
}
