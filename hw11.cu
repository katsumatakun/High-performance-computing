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

__global__ void swapArray( int *A, int size, int num_t) {
   int tid = blockIdx.x * blockDim.x + threadIdx.x;
  for(int i=size/2/num_t*tid; i<size/2/num_t*(tid+1); i++){
    int temp = A[i];
    A[i] = A[size-1-i];
    A[size-1-i] = temp;
  }
}

__global__ void mmmul(int N, float *A, float *B, float *C){
  //printf("working");
  //printf("A[1]: %d", A[1]);
  int T = 8;
  __shared__ float smem_c[64][64];
  if(threadIdx.x==0 && threadIdx.y==0){
    for (int i=0; i<64; i++){
      for(int j=0; j<64; j++){
        smem_c[i][j]=0.0;
    }
  }
}

  __shared__ float smem_a[64][8];
  __shared__ float smem_b[8][64];
  //printf("%f", smem_c[0][0]);
  int c = blockIdx.x * 64;
  int r = blockIdx.y * 64;
  //printf("%d \n",blockDim.x);
  //printf("%d \n", blockDim.y);
  if(threadIdx.y==31){
	//printf("x: %d \n",threadIdx.x);
}
  int cthread = c + threadIdx.x * 2;
  int rthread = r + threadIdx.y * 2;
  if(threadIdx.x==1 && threadIdx.y==1){
     //printf("==========r, c; %d, %d\n", rthread, cthread);
}
  int count= 0;
  for (int kk=0; kk<N; kk+=T) {
    for (int i=threadIdx.x+blockDim.x*threadIdx.y; i<64*8; i+=blockDim.x*blockDim.y) {
    int k = kk +i / 64;
    int rt = r +i % 64;
    int ct = c +i % 64;
    smem_a[i%64][i/64] = A[rt*N+k];
    smem_b[i/64][i%64] = B[k*N+ct];
    //smem_c[i%64][i%64] = 0;
    //printf("%d %d \n", smem_a[i%64][i/64], smem_b[i/64][i%64]);
    //printf("%d %d \n", A[rt*N+k], B[k*N+ct]);
  }
  // if(threadIdx.x==0 && blockIdx.x==0 && threadIdx.y==0 && blockIdx.y==0){
  //   printf("K: %d",k);
  //   for (int i=0; i<64; i++){
  //     for(int j=0; j<8; j++){
  //       if(smem_a[i][j]!=1 || smem_b[j][i] !=1){
  //         printf("incorect, a: %d, b: %d\n",smem_a[i][j], smem_b[i][j]);
  //       }
  //     }
  //   }
  // }
    __syncthreads();
    //printf("finish loading %f \n", smem_c[0][0]);
    for (int x=0; x<2; x++){
      //printf("x loop \n");
      for (int y=0; y<2; y++){
        //printf("y loop");
        int cc = cthread % 64 + x;
        int rc = rthread % 64 + y;
if(threadIdx.x==0 && blockIdx.x==0 && threadIdx.y==0 && blockIdx.y==0)
        printf("rc,cc = %d,%d\n",rc,cc );
        for (int k=kk; k<kk+T; k++){

          int k8 = k % T;
          //int rc64 = rc % 64;
          //int cc64 = cc % 64;
        float temp = smem_a[rc][k8] * smem_b[k8][cc];

          atomicAdd(&smem_c[rc][cc],temp);
          //smem_c[rc][cc]+=temp;
          if(threadIdx.x==0 && blockIdx.x==0 && threadIdx.y==0 && blockIdx.y==0){
            //printf("temp=%f\n",temp );
            //printf("rc,cc=%f\n", smem_c[rc][cc]);
          }
          count+=1;
          //printf("count %d \n", count);
          //printf("load into c \n");

	}
      }
    }

    __syncthreads();
    if(threadIdx.x==0 && blockIdx.x==0 && threadIdx.y==0 && blockIdx.y==0){
      printf("K: %d\n",kk);
      for (int i=0; i<2; i++){
        for(int j=0; j<2; j++){
          printf("%f ", smem_c[i][j]);
        }
        printf("\n");
      }
    }
     __syncthreads();
  }
    __syncthreads();
    //printf("working \n");
    for (int p=0; p<2; p++){
    for (int q=0; q<2; q++) {
      int ccc = cthread + p;
      int rcc = rthread + q;
      /*if(smem_c[rcc%64][ccc%64]!=1024.0){
//if(rcc%64 != 31 && rcc%64 !=30 && rcc%64 !=29)
printf("row, col: %d %d \n", rcc, ccc);
}*/
      C[rcc*N+ccc] = smem_c[rcc%64][ccc%64];
    }
}
    __syncthreads();
    //printf("C %d\n", C[0]);


}

//double func(int N){
//   // int N = 1024;
//   srand(time(0));
//   int size=N*N;
//   //int size=16;
//   float* A = new float[size];
//   float* B = new float[size];
//   float* C = new float[size];
//   float* Cseq = new float[size];
//
//   int num;
//   for(int i=0; i<size; i++){
//     num = rand()%100;
//     A[i] = num;
//     B[i] = num;
//   }
//
// /*  for(int i=0; i<N; i++){
//     for(int j=0; j<N; j++){
//       for(int k=0; k<N; k++){
//         Cseq[i*N+j] += A[i*N+k]*B[k*N+j];
//       }
//     }
//   }*/
//
//
//
//   float *d_a;
//   float *d_b;
//   float *d_c;
//
//   // define thread hierarchy
//   int num_blocks_x = N/64; int num_blocks_y = N/64; int num_th_per_blk = 32;
//   // int num_t = num_blocks_x*num_blocks_y*num_th_per_blk;
//
//   // allocate host and device memory
//   size_t memSize;
//   // memSize = num_blocks * num_th_per_blk * sizeof(int);
//   memSize = size*sizeof(int);
//   // h_a = (int*) malloc(memSize);
//   clock_t start = clock();
//
//   cudaMalloc( (void**) &d_a, memSize);
//   cudaMemcpy( d_a, A, memSize, cudaMemcpyHostToDevice);
//   cudaMalloc( (void**) &d_b, memSize);
//   cudaMemcpy( d_b, B, memSize, cudaMemcpyHostToDevice);
//   cudaMalloc( (void**) &d_c, memSize);
//   cudaMemcpy( d_c, C, memSize, cudaMemcpyHostToDevice);
//
//   // launch kernel
//   dim3 dimGrid2D(num_blocks_x, num_blocks_y);
//   dim3 dimBlock(num_th_per_blk, num_th_per_blk);
//   mmmul<<< dimGrid2D, dimBlock >>>(N, d_a, d_b, d_c);
//   cudaMemcpy( C, d_c, memSize, cudaMemcpyDeviceToHost);
//   clock_t end = clock();
//
//   double time_elapsed_in_seconds = (double)(end - start)/CLOCKS_PER_SEC;
//
//
//
//   /*for(int i=0; i<N*N; i+=1){
//     if(C[i]!=Cseq[i]){
//     printf("%f %f %d\n",Cseq[i],C[i], i);
// }
// }*/

// return time_elapsed_in_seconds;

//}


int main(int argc, char *argv[]){

  int N = 1024*4;
  srand(time(0));
  int size=N*N;
  //int size=16;
  float* A = new float[size];
  float* B = new float[size];
  float* C = new float[size];
  float* Cseq = new float[size];

  float num;
  for(int i=0; i<size; i++){
    num = rand()%10;
    A[i] = num;
    B[i] = num;
  }

for(int i=0; i<N; i++){
    for(int j=0; j<N; j++){
      for(int k=0; k<N; k++){
        Cseq[i*N+j] += A[i*N+k]*B[k*N+j];
      }
    }
  }



  float *d_a;
  float *d_b;
  float *d_c;

  // define thread hierarchy
  int num_blocks_x = N/64; int num_blocks_y = N/64; int num_th_per_blk = 32;
  // int num_t = num_blocks_x*num_blocks_y*num_th_per_blk;

  // allocate host and device memory
  size_t memSize;
  // memSize = num_blocks * num_th_per_blk * sizeof(int);
  memSize = size*sizeof(int);
  // h_a = (int*) malloc(memSize);
  clock_t start = clock();

  cudaMalloc( (void**) &d_a, memSize);
  cudaMemcpy( d_a, A, memSize, cudaMemcpyHostToDevice);
  cudaMalloc( (void**) &d_b, memSize);
  cudaMemcpy( d_b, B, memSize, cudaMemcpyHostToDevice);
  cudaMalloc( (void**) &d_c, memSize);
  cudaMemcpy( d_c, C, memSize, cudaMemcpyHostToDevice);

  // launch kernel
  dim3 dimGrid2D(num_blocks_x, num_blocks_y);
  dim3 dimBlock(num_th_per_blk, num_th_per_blk);
  mmmul<<< dimGrid2D, dimBlock >>>(N, d_a, d_b, d_c);
  cudaMemcpy( C, d_c, memSize, cudaMemcpyDeviceToHost);
  clock_t end = clock();

  double time_elapsed_in_seconds = (double)(end - start)/CLOCKS_PER_SEC;



  for(int i=0; i<N*N; i+=1){
    if(C[i]!=Cseq[i]){
    printf("%f %f %d\n",Cseq[i],C[i], i);
}
//if(C[i]!= 4096.0){
//printf("C[i]:%f, i:%d", C[i], i);
//}
}

  // for(int i=1; i<3; i++){
    printf("execution time for N = %d was %f\n", N, time_elapsed_in_seconds);
  // }
  return 0;
}
