#include "stdio.h"
#include "stdlib.h"

int main()
{  
#pragma omp parallel
     printf("Hola mundo\n");
exit (0);
}
