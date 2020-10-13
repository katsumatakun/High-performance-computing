#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// 456631

int block_index(int, int, int);

int main(int argc, char* argv[]){

  FILE *fp;
  // if((fp = fopen("test.txt", "r")) == NULL ) {
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

  int n = ptr[0];
  int num_data = ptr[2];

  int* col_index = (int*) (malloc(sizeof(int)*num_data));
  int* data = (int*) (malloc(sizeof(int)*num_data));
  int* invec = (int*) (malloc(sizeof(int)*n));
  for (int i=0; i<n; i++){
    invec[i] = 1;
  }
  int* rowPtr = (int*) (malloc(sizeof(int)*(n+1)));
  int* outvec = (int*) (malloc(sizeof(int)*n));
  int* outvec2 = (int*) (malloc(sizeof(int)*n));
  int* ans = (int*) (malloc(sizeof(int)*n));
  printf("=======\n");

  int block_size = 8192;
  // int block_size = 49;
  int n_blocks = (n+block_size-1)/block_size;
  // int n_blocks = n/block_size;
  int* data_block = (int*) (malloc(sizeof(int)*num_data));
  int num_each_block[n_blocks];
  for (int i=0; i<n_blocks; i++ )
    num_each_block[i]=0;

  int count = 0;
  int row = 0;
  rowPtr[0] = 0;
  printf("=======\n");


  while(fscanf(fp,"%d",&ptr[0]) != EOF && fscanf(fp,"%d",&ptr[1]) != EOF && fscanf(fp, "%d", &ptr[2]) != EOF){
    // printf("%d %d %d\n", ptr[0], ptr[1], ptr[2]);
    printf("row:%d col:%d\n", ptr[0], ptr[1]);
    num_each_block[ptr[1]/block_size]++;
    if(ptr[0]==row){
      col_index[count] = ptr[1];
      data[count] = ptr[2];
    }
    else{
      do{
        row++;
        rowPtr[row] = count;
        ans[row-1]=rowPtr[row]-rowPtr[row-1];
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
    ans[j-1]=rowPtr[j]-rowPtr[j-1];
}

 fseek( fp, 0, SEEK_SET );
 for (int i=0; i<3; i++){
   fscanf(fp,"%d",&num);
   printf("%d\n", num);
 }
 row = 0;
 int block=0;
 int* counts = (int*) malloc(sizeof(int)*n_blocks);
 int* col_index_block = (int*) (malloc(sizeof(int)*num_data));
 int *row_ptr_block[n_blocks];
   for (int i=0; i<n_blocks; i++)
        row_ptr_block[i] = (int *)malloc((n+1) * sizeof(int));
 num=0;
 for(int i=0; i<n_blocks; i++){
   for(int j=0; j<=i; j++){
     num+= num_each_block[j];
   }
   row_ptr_block[i][n]=num;
   if(i<n_blocks-1)
    row_ptr_block[i+1][0]=num;
   // printf("offset: %d\n", num);
   num=0;
 }
 int offset;

 while(fscanf(fp,"%d",&ptr[0]) != EOF && fscanf(fp,"%d",&ptr[1]) != EOF && fscanf(fp, "%d", &ptr[2]) != EOF){
   block = ptr[1]/block_size;
   if(block>0){
     offset = row_ptr_block[block-1][n];
   }
   else{
     offset=0;
   }
   printf("row: %d, col: %d, offset: %d, col_idx %d\n", ptr[0],ptr[1], offset,counts[block]+offset);
   if(ptr[0]==row){
     col_index_block[counts[block]+offset] = ptr[1];
     data_block[counts[block]+offset] = ptr[2];
   }
   else{
       do{
       row++;
       row_ptr_block[0][row]=counts[0];
       for (int i=1; i<n_blocks; i++){
         row_ptr_block[i][row]=counts[i]+row_ptr_block[i-1][n];
       }
     }while(row!=ptr[0]);
     col_index_block[counts[block]+offset] = ptr[1];
     data_block[counts[block]+offset] = ptr[2];
   }
   counts[block]++;
 }

 int col;
 int val;
 int i;
 int j;
 int x;


  clock_t start = clock();
  for (i=0; i<n; i++) {
    outvec[i] = 0;
    for (j=rowPtr[i]; j<rowPtr[i+1]; j++){
      col = col_index[j];
      val = data[j];
      outvec[i] += val * invec[col];
    }
  }
  clock_t end = clock();
  double time_elapsed_in_seconds = (double)(end - start)/CLOCKS_PER_SEC;
  printf("normal: %f\n",time_elapsed_in_seconds);


  clock_t start2 = clock();
  for(x=0; x<n_blocks; x++){
    for (i=0; i<n; i++) {
      for (j=row_ptr_block[x][i]; j<row_ptr_block[x][i+1]; j++){
        col = col_index_block[j];
        val = data_block[j];
        outvec2[i] += val * invec[col];
      }
    }
  }
  clock_t end2 = clock();
  double time_elapsed_in_seconds2 = (double)(end2 - start2)/CLOCKS_PER_SEC;
  printf("blocking: %f\n",time_elapsed_in_seconds2);

  // int match = 1;
  // for (int i=0; i<n; i++){
  //   if(outvec[i]!=ans[i] || outvec2[i]!=ans[i]){
  //     match=0;
  //     printf("not match: %d, %d, %d, %d\n",i,ans[i], outvec[i],outvec2[i] );
  //   }
  // }
  // printf("%d\n",match );
  // for(int i=0; i<10; i++){
  //   printf("%d, %d, %d\n",outvec[i], outvec2[i], ans[i] );
  // }
  // for (int i=0; i<num_data; i++){
  //   // printf("col index %d, %d \n", i, col_index_block[i]);
  //   printf("col idx %d\n",col_index_block[i]);
  // }
  // for(int x=0; x<n_blocks; x++){
  //   for (int i=0; i<n; i++) {
  //     printf("block: %d, row: %d, rowptr %d\n", x, i, row_ptr_block[x][i]);
  //   }
  // }

  // printf("%d %d %d\n", ptr[0], ptr[1], ptr[2]);
  printf("EOF %d\n",fscanf(fp,"%d",&ptr[0])==EOF);
  return 0;

}
