#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>

// Creates an array of random numbers. Each number has a value from 0 - 1
int * read_array(int num_elements) {
  int *array = (int *)malloc(sizeof(int) * num_elements);
  assert(array != NULL);
  int i;
  for (i = 0; i < num_elements; i++)
      scanf("%d", &array[i]);
  return array;
}

// Computes the average of an array of numbers
int * run_simulation(int *row, int *xs, int *ys, int *rs, int *ps, int N,
                     int B) {
    int *results = (int *)malloc(sizeof(int) * 6);
    int i, mt_kill, mt_hit, mt_intact, ct_kill, ct_hit, ct_intact;
    mt_kill = mt_hit = mt_intact = ct_kill = ct_hit = ct_intact = 0;

    for (i=0; i<B; i++){
        // analyze attack on this row
    }

    return results;
}

int *accumulate_simulations(int *array, int num_elements) {
    int *results = (int *)malloc(sizeof(int) * 6);

    int i;
    for (i = 0; i < num_elements; i+=6) {
        results[0] += array[i];
        results[1] += array[i+1];
        results[2] += array[i+2];
        results[3] += array[i+3];
        results[4] += array[i+4];
        results[5] += array[i+5];
    }
  return results;
}

int *read_map(int N){
    int i, x, y, target_value;
    int *war_map = (int *)malloc(sizeof(int) * N * N);
    assert(war_map != NULL);

    scanf("%d", &T);

    for (i = 0; i < T; i++) {
        scanf("%d %d %d", &x, &y, &target_value);
        war_map[x*N + y] = target_value;
    }

    return war_map;
}

void read_attacks(int B, int *xs, int *ys, int *rs, int *ps) {
    int i, x, y, r, p;
    xs = (int *)malloc(sizeof(int) * B);
    assert(xs != NULL);
    ys = (int *)malloc(sizeof(int) * B);
    assert(ys != NULL);
    rs = (int *)malloc(sizeof(int) * B);
    assert(rs != NULL);
    ps = (int *)malloc(sizeof(int) * B);
    assert(ps != NULL);

    for (i=0; i<B; i++) {
        scanf("%d %d %d %d", &x, &y, &r, &p);
        xs[i] = x;
        ys[i] = y;
        rs[i] = r;
        ps[i] = p;
    }
}

int main(int argc, char** argv) {
    int world_rank, world_size, N, B;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // read input on the root process
    int *war_map = NULL, *xs = NULL, *ys = NULL, *rs = NULL, *ps = NULL;

    if (world_rank == 0) {
        scanf("%d", &N);
        war_map = read_map(N);
        int i, j;
        for (i=0; i<N; i++) {
            for (j=0; j<N; j++)
                printf("%d ", war_map[i*N + j]);
            printf("\n");
        }

        scanf("%d", &B);
        read_attacks(B, xs, ys, rs, ps);

        // send attacks arrays to other processes
        for (i=0; i<world_size; i++) {
            MPI_Send(xs, B, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(ys, B, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(rs, B, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(ps, B, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Status status;
        MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&B, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(xs, B, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(ys, B, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(rs, B, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(ps, B, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }

    // For each process, create a buffer that will hold a subset of the entire
    // array
    int *sub_war_map = (int *)malloc(sizeof(int) * N / world_size);
    assert(sub_war_map != NULL);

    // Scatter the random numbers from the root process to all processes in
    // the MPI world
    MPI_Scatter(war_map, N / world_size, MPI_INT, sub_war_map, N / world_size,
                MPI_INT, 0, MPI_COMM_WORLD);

    // Compute the average of your subset
    int* sub_war_result = run_simulation(sub_war_map, xs, ys, rs, ps,
                                         N / world_size, B);
    printf("I am the process %d and my war results are: (%d %d %d %d %d %d)\n",
           world_rank, sub_war_result[0], sub_war_result[1], sub_war_result[2],
           sub_war_result[3], sub_war_result[4], sub_war_result[5]);
  
    // Gather all partial averages down to the root process
    int *sub_war_results = NULL;
    if (world_rank == 0) {
        sub_war_results = (int *)malloc(sizeof(int) * 6 * world_size);
        assert(sub_war_results != NULL);
    }
    MPI_Gather(sub_war_result, 6, MPI_INT, sub_war_results, 6, MPI_INT, 0,
               MPI_COMM_WORLD);

    int *war_results, *original_results;
    if (world_rank == 0) {
        war_results = accumulate_simulations(sub_war_results, 6 * world_size);
        printf("%d %d %d %d %d %d\n",
               war_results[0], war_results[1], war_results[2],
               war_results[3], war_results[4], war_results[5]);

        // Compute the average across the original data for comparison
        original_results = run_simulation(war_map, xs, ys, rs, ps, N*N, B);
        printf("%d %d %d %d %d %d\n",
               original_results[0], original_results[1], original_results[2],
               original_results[3], original_results[4], original_results[5]);
    }

    // Clean up
    if (world_rank == 0) {
        free(war_map);
        free(sub_war_results);
        free(original_results);
        free(war_results);
    }
    free(sub_war_map);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
}
