#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>


#define INT_MAX 2147483647;
void* BellmanFord(void* tn);
void* test(void* tn);

int* invec;
int* temp;
int* invec2;
int* col_index;
int* data;
int* rowPtr;
int n;
// int flag=0;
int flag=0;
pthread_barrier_t barrier;
int num_threads;
int main(int argc, char* argv[]){

  FILE *fp;
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
  invec2 = (int*) (malloc(sizeof(int)*n));
  for (int i=0; i<n; i++){
    invec[i] = 2147483647;
    invec2[i] = 2147483647;
  }
  invec[0] = 0;
  invec2[0] = 0;
  rowPtr = (int*) (malloc(sizeof(int)*(n+1)));
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

  pthread_t *threads;
  void *th_status;
  char* cptr;
  printf("aaaaaa\n");
  if (argc>1){
    num_threads = (int) strtol(argv[1], &cptr, 10);
  }
  else{
    num_threads = 1;
  }
  // num_threads = 2;
  printf("%d\n",n );
  printf("num thread %d\n", num_threads);
  int* ids = (int*)malloc(sizeof(int)*num_threads);
  threads = (pthread_t*)malloc(sizeof(pthread_t)*num_threads);


  pthread_barrier_init(&barrier, NULL, num_threads);
  clock_t start = clock();
  for (int tn=0; tn<num_threads; tn++){
    ids[tn] = tn;
    int results = pthread_create(&threads[tn], NULL, BellmanFord, (void*)&ids[tn]);
    // int results = pthread_create(&threads[tn], NULL, test, (void*)&ids[tn]);
    // printf("create %d\n", results);
  }

  // pthread_barrier_wait(&barrier);
  for (int tn=0; tn<num_threads; tn++){
    int join = pthread_join(threads[tn], NULL);
    // printf("join %d\n", join);
  }
  clock_t end = clock();
  pthread_barrier_destroy(&barrier);
  double time_elapsed_in_seconds = (double)(end - start)/CLOCKS_PER_SEC;
  printf("%d threads time: %f\n",num_threads,time_elapsed_in_seconds);


  printf("%d threads 100: %d, %d\n",num_threads, invec[100], invec[0]);
  free(ids);
  free(threads);
  // for (int i=0; i<n; i++){
  //   invec[i] = 2147483647;
  //   invec2[i] = 2147483647;
  // }
  // invec[0] = 0;
  // invec2[0] = 0;
  printf("%d threads done\n", num_threads);






// int* temp;
// int flag;
// int ans = 0;
//   for (int iter=0; iter<n; iter++){
//     printf("iter\n");
//     flag = 0;
//     for (int i = 0; i < n ; i++) {
//       for (int j = rowPtr[i]; j < rowPtr[i+1]; j++) {
//           int v = col_index[j];
//           int weight = data[j];
//           if (invec[v] != 2147483647 && invec[v] + weight < invec2[i]){
//               flag=1;
//               invec2[i] = invec[v] + weight;
//             }
//           }
//         }
//         if(!flag)
//           break;
//         temp = invec; invec = invec2; invec2=temp;
//   }
  printf("100: %d, %d\n", invec[100], invec[0]);
}

void* test(void* tn){
  printf("test working\n");
  flag+=1;
  printf("flag %d\n", flag);
  pthread_exit(NULL);
}

void* BellmanFord(void* tn){
  // printf("working");
  int bsize = n/num_threads;
  int tid = *((int*) tn);
  // printf("%d", tid);
  // int flag;
  // pthread_barrier_wait(&barrier);
  for (int k=0; k<n; k++){
    // printf("iter");
    flag = 0;
    for (int i=tid*bsize; i< (tid+1)*bsize; i++) {
      for (int j = rowPtr[i]; j < rowPtr[i+1]; j++) {
          int v = col_index[j];
          int weight = data[j];
          if (invec[v] != 2147483647 && invec[v] + weight < invec2[i]){
              // flag=1;
              invec2[i] = invec[v] + weight;
            }
          }
        }
        pthread_barrier_wait(&barrier);
        // printf("id %d\n", tid);
        if(tid==0){
        // printf("merge\n" );
        for(int i=0; i<n; i++){
          if(invec[i]!=invec2[i])
            flag = 1;
          invec[i] = invec2[i];
        }
      }
      // for(int i=0; i<n; i++){
      //     invec[i] = invec2[i];
      //   }
          // temp = invec; invec = invec2; invec2=temp;
        pthread_barrier_wait(&barrier2);

        // printf("flag %d id %d\n",flag, tid );
        if(!flag){
          // printf("break %d\n", tid);
          // pthread_barrier_wait(&barrier);
          break;
        }
        pthread_barrier_wait(&barrier2);
    }
    pthread_exit(NULL);
}
