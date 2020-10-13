#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(int argc, char* argv[]){

  FILE *fp;
  // if((fp = fopen("test.txt", "r")) == NULL ) {
  //   printf("Input file not open\n");
  //   return -1;
  // }
  if((fp = fopen("matrix.txt", "w")) == NULL ) {
    printf("Input file not open\n");
    return -1;
  }

  int num1 = atoi(argv[1]);
  int num2 = atoi(argv[2]);
  fprintf(fp,"%d %d %d\n", num1, num1, num2);
  int i, n;
  time_t t;


   /* Intializes random number generator */
   srand((unsigned) time(&t));
   int num3 = 1;
   /* Print 5 random numbers from 0 to 49 */

   int row=0;
   int col=0;
   int* rows = (int*) malloc(sizeof(int)*num2);
   int* cols = (int*) malloc(sizeof(int)*num2);
   printf("==========\n");
   printf("%d\n", num2);
   for( i = 0 ; i < num2 ; i++ ) {

      rows[i] = rand() % num1;
      cols[i] = rand() % num1;
   }

   for (i = 0; i < num2; ++i)
        {
            for (int j = i + 1; j < num2; ++j)
            {
                if (rows[i] > rows[j])
                {
                    int a =  rows[i];
                    rows[i] = rows[j];
                    rows[j] = a;
                }
            }
        }

        int count=0;
        while(count<num1){
          row = rows[count];
          int copy = count;
          while(row==rows[copy] && copy<num1){
            copy++;
          }
          for (i = count; i < copy; ++i)
               {
                   for (int j = i + 1; j < copy; ++j)
                   {
                       if (cols[i] > cols[j])
                       {
                           int a =  cols[i];
                           cols[i] = cols[j];
                           cols[j] = a;
                       }
                   }
               }
            count = copy;

          // fprintf(fp, "%d %d %d\n", rows[count], cols[count], 1 );
        }

        for(i=0; i<num2; i++){
          fprintf(fp, "%d %d %d\n", rows[i], cols[i], 1 );
        }
        fclose(fp);


}
