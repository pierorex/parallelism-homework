/*
-----------------------------------------------------------------------
Code:
choose.c
Lab:
MPI Derived Datatypes
Usage: choose
------------------------------------------------------------------------ 
*/
#include "mpi.h"
#define IMAX 16      // rows
#define JMAX 16		 // columns
#define SOURCERANK 0 // master node

main (int argc, char **argv) {
	int i, j, myrank, count=0, num_procs;
	MPI_Status status;
	MPI_Datatype coltype;
	int matrix [IMAX][JMAX];

	/* Inicializacion del ambiente MPI*/
	MPI_Init(&argc, &argv);
    MPI_Comm_size (MPI_COMM_WORLD, &num_procs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	printf ( "Proceso %d inicializado \n", myrank );

	/* Inicializacion de la matriz */
	if ( myrank == SOURCERANK )
	for ( i=0; i<IMAX; i++)
		for (j=0; j<JMAX; j++)
			matrix[i][j] = count++;
	else
		for ( i=0; i<IMAX; i++)
			for (j=0; j<JMAX; j++)
				matrix[i][j] = 0;

	/* Definicion del nuevo tipo de datos derivado segun el ejercicio */
	MPI_Type_vector(JMAX / 2, 1, 2, MPI_INT, &coltype);
	MPI_Type_commit(&coltype);

	if ( myrank == SOURCERANK ) {
		/* El proceso SOURCERANK reparte la matriz usando el nuevo tipo de dato
		derivado definido */ 
		for (i=0; i<IMAX; i++) {
			if (i % 2 == 0)
				MPI_Send(&matrix[i][0], 1, coltype, 1, 0, MPI_COMM_WORLD);
			else
				MPI_Send(&matrix[i][1], 1, coltype, 1, 0, MPI_COMM_WORLD);
		}
	}
	else {
		/* el resto de los procesos recibe la porción de la matriz que le
		corresponde */
		MPI_Request request;
		for (i=0; i<IMAX; i++) {
			if (i % 2 == 0)
				MPI_Recv(&matrix[i][0], 1, coltype, SOURCERANK, 0, MPI_COMM_WORLD, &status);
			else
				MPI_Recv(&matrix[i][1], 1, coltype, SOURCERANK, 0, MPI_COMM_WORLD, &status);
		}

		/* imprime la matriz completa*/
		printf("\n");
		for (i=0; i<IMAX; i++) {
			for (j=0; j<JMAX; j++) {
				printf("     %d", matrix[i][j]);
			}
			printf("\n");
		}
	}
	MPI_Type_free(&coltype);
	MPI_Finalize();
}
