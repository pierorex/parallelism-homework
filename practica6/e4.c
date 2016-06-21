/* Programa e4.c */
#include "stdio.h"
#include "stdlib.h"
#define N 4

float producto(float* a, float* b, int n) { 
   float sum = 0.0; 
   int i;
   #pragma omp parallel for shared(sum)
      for  (i=0; i<n; i++){
         sum += a[i] * b[i]; 
      } 
      return sum;
}


int main()
{
   float total=0; 
   float a[N],b[N];
   int k=0;
   for (k=0;k<N;++k) {
     a[k]=k;
     b[k]=k;
   }
   total = producto(a,b,N);

   printf("%.6f \n",total);

   int i,s,r,t;
   for (i=0;i<N;i++){
      s=a[i];
      r=b[i];
      t=a[i] * b[i];
      printf("%d ",r);
      printf(" * %d ",s);
      printf(" = %d \n",t);
   }
   exit (0);
}

