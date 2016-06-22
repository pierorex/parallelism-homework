#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

int main( int argc, char *argv[] ) {
    long i, iterations, count;
    double x, y, pi;

    if (argc != 2) {
        printf ("Uso: piUno numIteraciones \n");
        exit(0);
    }

    iterations = atoi(argv[1]);
    count = 0;
    
    for (i=0; i<iterations; i++) {
        x = (double) random() / (double) RAND_MAX;
        y = (double) random() / (double) RAND_MAX; 
        if ((x*x + y*y) <= 1.0) count++;
    }
    pi = (4.0 * count) / (double) iterations;
    printf ("Valor de PI = %.6lf\n", pi);
    printf ("Error = %.6lf\n", fabs(pi-M_PI));
} 