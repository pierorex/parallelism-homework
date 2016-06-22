#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define N 800

float A[N][N], B[N][N], C[N][N];

int main ()
{
   int i, j, m;
   
   srand (time(NULL));
   for(i=0; i<N; i++) {
      for(j=0; j<N; j++) {
         A[i][j] = (rand()%10);
         B[i][j] = (rand()%10);
      }
   }

   for(i=0; i<N; i++) {
      for(j=0; j<N; j++) {
         C[i][j]=0.0;
         for(m=0; m<N; m++) {
            C[i][j] += A[i][m]*B[m][j];
         }
      }
    }
}

