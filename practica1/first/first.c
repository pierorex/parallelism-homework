#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int me, numProcess;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &me);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcess);
    if (me == 0) printf("%d procesos creados\n", numProcess);
    printf("Soy el proceso %d\n", me);
    MPI_Finalize();
    return 0;
}
