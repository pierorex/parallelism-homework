//------------------------------------------------------------------+
//                                                                  |
// CuentaPar.c:  Cuenta aparaciones de un numero en un arreglo muy  |
//               grande. Version paralela simple                    |
//                           ESQUELETO                              |
//------------------------------------------------------------------+

#include <assert.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"
#define MAX_ENTERO      1000
#define NUM_VECTORES   10000  // Simula vector todavia mayor
#define NUM_BUSCADO        5

//--------------------------------------------------------------------
int esclavo(int numprocs, int cardinality, int yo){
  int  *vector, 
	segment_length = cardinality / numprocs, 
        numVeces, 
	i,
	j;
  MPI_Status status;
  //struct timeval t0, tf, t;

  assert((vector = (int *)malloc(sizeof(int)*cardinality)) != NULL);
  //printf("%d receiving\n", yo);
  MPI_Recv(vector, segment_length, MPI_INT, 0, 0, MPI_COMM_WORLD, &status); 
  //for(i=0; i<segment_length; i++) printf("%d ", vector[i]);
  //printf("\n%d received, segment_length = %d\n", yo, segment_length);
  numVeces = 0;
  //assert(gettimeofday(&t0, NULL) == 0);

  for (i=0; i<NUM_VECTORES; i++) {
    for (j=0; j<segment_length; j++)
      if (vector[j] == NUM_BUSCADO) numVeces++;
  }
  //assert(gettimeofday(&tf, NULL) == 0);
  //timersub(&tf, &t0, &t);
  //printf ("Computing time in %d = %ld:%3ld\n", yo, t.tv_sec, t.tv_usec/1000);
  return numVeces;
}

//--------------------------------------------------------------------
int maestro (int NumProcesos, int Cardinalidad) {

  int i, j, proc, totalNumVeces, numVeces, segment_length = Cardinalidad / NumProcesos;
  //struct timeval t0, tf, t;
  int *vector;

  // Inicializar el vector
  assert((vector = (int *)malloc(sizeof(int)*Cardinalidad)) != NULL);

  //printf("vector:");
  for (i=0; i<Cardinalidad; i++) {
    vector[i] = random() % MAX_ENTERO;
    //printf("%d ", vector[i]);
  }
  //printf("\n");

  // Repartir trabajo
  segment_length = Cardinalidad / NumProcesos;  

  for(i=0, proc=1; proc < NumProcesos; i += segment_length, proc++) {
    //printf("Sending segment (%d:%d) to %d\n", i, i + segment_length, proc);
    //assert(gettimeofday(&t0, NULL) == 0);
    MPI_Send(vector + i, segment_length, MPI_INT, proc, 0, MPI_COMM_WORLD);
    //assert(gettimeofday(&tf, NULL) == 0);
    //timersub(&tf, &t0, &t);
    //printf ("'Sending' time = %ld:%3ld\n", t.tv_sec, t.tv_usec/1000);
  }

  // Computar mi trozo
  //printf("Computing segment (%d:%d) in 0\n", (NumProcesos-1)*segment_length, Cardinalidad);
  numVeces = 0;

  //assert(gettimeofday(&t0, NULL) == 0);
  for (i=0; i<NUM_VECTORES; i++)
    for (j=(NumProcesos-1)*segment_length; j<Cardinalidad; j++)
      if (vector[j] == NUM_BUSCADO) numVeces++;
  //assert(gettimeofday(&tf, NULL) == 0);
  //timersub(&tf, &t0, &t);
  //printf ("Computing time in %d = %ld:%3ld\n", 0, t.tv_sec, t.tv_usec/1000);
  return numVeces;
}

//--------------------------------------------------------------------
int main( int   argc, char *argv[] ) {
  int yo, numProcesos, total=0, numVeces, laCardinalidad;
  struct timeval t0, tf, t;

  if (argc != 2) {
    printf ("Uso: cuentaPar cardinalidad \n");
    return 0;
  }
  laCardinalidad = atoi(argv[1]);
  assert(gettimeofday(&t0, NULL) == 0);
  assert (laCardinalidad > 0);
  setbuf (stdout, NULL);
  MPI_Init (&argc, &argv);
  MPI_Comm_rank (MPI_COMM_WORLD, &yo);
  MPI_Comm_size (MPI_COMM_WORLD, &numProcesos);
  //printf("%d Here\n", yo);
  if (yo == 0) numVeces = maestro(numProcesos, laCardinalidad);
  else         numVeces = esclavo(numProcesos, laCardinalidad, yo);
  //printf("%d counted %d\n", yo, numVeces);
  MPI_Reduce(&numVeces, &total, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  if (yo == 0) {
    assert(gettimeofday(&tf, NULL) == 0);
    timersub(&tf, &t0, &t);
    printf ("Numero de veces que aparece el %d = %d\n", NUM_BUSCADO, total);
    printf ("tiempo total = %ld:%3ld\n", t.tv_sec, t.tv_usec/1000);
  }
  MPI_Finalize();
  return 0;
}
