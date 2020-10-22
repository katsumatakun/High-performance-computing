#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

//
// int* invec;
// int* temp;
// int* outvec;
// int* col_index;
// int* data;
// int* rowPtr;
// int n;
// int num_threads;
int main(int argc, char* argv[]){

  FILE *fp;

  int* invec;
  int* temp;
  int* outvec;
  int* col_index;
  int* data;
  int* rowPtr;
  int n;
  int num_threads;
  // if((fp = fopen("matrix.txt", "r")) == NULL ) {
  //   printf("Input file not open\n");
  //   return -1;
  // }
  if((fp = fopen("higgs-twitter.mtx", "r")) == NULL ) {
    printf("Input file not open\n");
    return -1;
  }

  // printf("%s\n",argv[1]);
  // printf("%d\n",block_index(11,4,8));
  // return 1;
  printf("=======\n");
  int ptr[3];
  int num;
  for (int i=0; i<3; i++){
    fscanf(fp,"%d",&num);
    // printf("%d\n", a);
    ptr[i] = num;
  }

  n = ptr[0];
  int num_data = ptr[2];




  col_index = (int*) (malloc(sizeof(int)*num_data));
  data = (int*) (malloc(sizeof(int)*num_data));
  invec = (int*) (malloc(sizeof(int)*n));
  outvec = (int*) (malloc(sizeof(int)*n));
  for (int i=0; i<n; i++){
    invec[i] = 1;
    outvec[i] = 0;
  }
  rowPtr = (int*) (malloc(sizeof(int)*(n+1)));
  int* ans = (int*) (malloc(sizeof(int)*n));
  printf("=======\n");

  int count = 0;
  int row = 0;
  rowPtr[0] = 0;
  printf("=======\n");


  while(fscanf(fp,"%d",&ptr[0]) != EOF && fscanf(fp,"%d",&ptr[1]) != EOF && fscanf(fp, "%d", &ptr[2]) != EOF){
    // printf("%d %d %d\n", ptr[0], ptr[1], ptr[2]);
    // printf("row:%d col:%d\n", ptr[0], ptr[1]);
    // if(ptr[1]==0)
      // printf("%d\n",ptr[1]);
    if(ptr[0]==row){
      col_index[count] = ptr[1];
      data[count] = ptr[2];
    }
    else{
      do{
        row++;
        rowPtr[row] = count;
        // ans[row-1]=rowPtr[row]-rowPtr[row-1];
          // row_ptr_block[i][row] = counts[i];
        }while(row!=ptr[0]);

      // printf("=========\n");
      col_index[count] = ptr[1];
      data[count] = ptr[2];

    }
    count++;
  }
  // printf("row %d, count %d block %d\n",row,count,block_count );
  for(int j=row+1; j<=n; j++){
    rowPtr[j] = count;
    // ans[j-1]=rowPtr[j]-rowPtr[j-1];
}

  char* cptr;
  printf("aaaaaa\n");
  if (argc>1){
    num_threads = (int) strtol(argv[1], &cptr, 10);
  }
  else{
    num_threads = 1;
  }
  // num_threads = 2;
  int col;
  int val;
  int i;
  int j;
  printf("num_threads %d\n", num_threads);
  clock_t start = clock();
  #pragma omp parallel for num_threads(num_threads)
  {
    for (i=0; i<n; i++) {
      int sum=0;
      for (j=rowPtr[i]; j<rowPtr[i+1]; j++){
        col = col_index[j];
        val = data[j];
        sum += val * invec[col];
      }
      outvec[i] = sum;
    }
 }

   clock_t end = clock();
   double time_elapsed_in_seconds = (double)(end - start)/CLOCKS_PER_SEC;
   printf("num_threads: %d, time: %f\n",num_threads, time_elapsed_in_seconds);
   printf("out[40]: %d\n", outvec[40]);
   // printf("test %d %d %d %d\n",outvec[100000], outvec[22222], outvec[9999], outvec[400000]);

   //
   // for (i=0; i<n; i++) {
   //   ans[i]=0;
   //   for (j=rowPtr[i]; j<rowPtr[i+1]; j++){
   //     col = col_index[j];
   //     val = data[j];
   //     ans[i] += val * invec[col];
   //   }
   // }
   // printf("ans[40]: %d\n", outvec[40], outvec[0]);

}
