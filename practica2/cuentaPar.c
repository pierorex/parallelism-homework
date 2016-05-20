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
  int *vector, 
	    segment_length = cardinality / numprocs, 
      numVeces, i, j;
  
  MPI_Status status;

  assert((vector = (int *)malloc(sizeof(int)*cardinality)) != NULL);
  MPI_Recv(vector, segment_length, MPI_INT, 0, 0, MPI_COMM_WORLD, &status); 
  numVeces = 0;

  // computation part
  for (i=0; i<NUM_VECTORES; i++) {
    for (j=0; j<segment_length; j++)
      // we make sure to only iterate while j<segment_length
      // to take advantage of parallel execution
      if (vector[j] == NUM_BUSCADO) numVeces++;
  }

  // return slave's answer to be accumulated in main
  return numVeces;
}

//--------------------------------------------------------------------
int maestro (int NumProcesos, int Cardinalidad) {

  int i, j, proc, totalNumVeces, numVeces, segment_length = Cardinalidad / NumProcesos;
  int *vector;

  // Inicializar el vector
  assert((vector = (int *)malloc(sizeof(int)*Cardinalidad)) != NULL);

  for (i=0; i<Cardinalidad; i++) {
    vector[i] = random() % MAX_ENTERO;
  }

  // Split work by domain
  segment_length = Cardinalidad / NumProcesos;  

  for(i=0, proc=1; proc < NumProcesos; i += segment_length, proc++) {
    MPI_Send(vector + i, segment_length, MPI_INT, proc, 0, MPI_COMM_WORLD);
  }

  // Compute master's slice
  numVeces = 0;

  for (i=0; i<NUM_VECTORES; i++)
    for (j=(NumProcesos-1)*segment_length; j<Cardinalidad; j++)
      if (vector[j] == NUM_BUSCADO) numVeces++;

  // return master's answer to be accumulated in main
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

  if (yo == 0) numVeces = maestro(numProcesos, laCardinalidad);
  else         numVeces = esclavo(numProcesos, laCardinalidad, yo);

  // sum all answers from processes into 'total'
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
