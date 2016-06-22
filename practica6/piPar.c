#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>

int main( int argc, char *argv[] ) {
    long i, iterations, count;
    double pi;

    if (argc != 2) {
        printf ("Uso: piPar numIteraciones \n");
        exit(0);
    }

    iterations = atoi(argv[1]);
    count = 0;
    
    #pragma omp parallel 
    {
        srand(time(NULL) * (1+omp_get_thread_num()));  // generate a different seed for each process
        #pragma omp for reduction(+:count)
        for (i=0; i<iterations; i++) {
            //printf("%d ", omp_get_thread_num());
            double x = (double) random() / (double) RAND_MAX;
            double y = (double) random() / (double) RAND_MAX; 
            if ((x*x + y*y) <= 1.0) count++;
        }
    }
    pi = (4.0 * count) / (double) iterations;
    printf ("Valor de PI = %.6lf\n", pi);
    printf ("Error = %.6lf\n", fabs(pi-M_PI));
}