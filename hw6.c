#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define INT_MAX 2147483647;

int main(int argc, char* argv[]){

  FILE *fp;
  if((fp = fopen("matrix.txt", "r")) == NULL ) {
    printf("Input file not open\n");
    return -1;
  }
  // if((fp = fopen("higgs-twitter.mtx", "r")) == NULL ) {
  //   printf("Input file not open\n");
  //   return -1;
  // }

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
  int* invec2 = (int*) (malloc(sizeof(int)*n));
  for (int i=0; i<n; i++){
    invec[i] = 2147483647;
    invec2[i] = 2147483647;
  }
  invec[0] = 0;
  int* rowPtr = (int*) (malloc(sizeof(int)*(n+1)));
  int* outvec2 = (int*) (malloc(sizeof(int)*n));
  // int* ans = (int*) (malloc(sizeof(int)*n));
  printf("=======\n");

  int count = 0;
  int row = 0;
  rowPtr[0] = 0;
  printf("=======\n");


  while(fscanf(fp,"%d",&ptr[0]) != EOF && fscanf(fp,"%d",&ptr[1]) != EOF && fscanf(fp, "%d", &ptr[2]) != EOF){
    // printf("%d %d %d\n", ptr[0], ptr[1], ptr[2]);
    // printf("row:%d col:%d\n", ptr[0], ptr[1]);
    if(ptr[1]==0)
      printf("%d\n",ptr[1]);
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

int* temp
for (int iter=0; iter<n; iter++){
  for (int i = 0; i < n ; i++) {
    for (int j = rowPtr[i]; j < rowPtr[i+1]; j++) {
        int v = col_index[j];
        int weight = data[j];
        if (invec[i] + weight < invec[v])
            invec[v] = invec[i] + weight;
          }
      }
      temp = invec;
}
  printf("100: %d\n", invec[3]);
}
