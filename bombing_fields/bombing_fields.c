#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>


int hit_target(int target_value, int attack_power) {
    if (target_value > 0) {
        if (attack_power >= target_value) return 0;
        else return target_value - attack_power;
    }
    else {
        if (attack_power >= -target_value) return 0;
        else return target_value + attack_power;
    }
}


bool inside_square(int x, int y, int left, int right, int top, int bottom) {
    // decide if a point (x,y) resides inside the 
    // square with boundaries (left, right, top, bottom)
    return x <= right && x >= left && y <= top && y >= bottom;
}


// Computes the average of an array of numbers
int *run_simulation(int T, int *targets, int B, int *attacks) {
    int i, j, mt_kill, mt_hit, mt_intact, ct_kill, ct_hit, ct_intact, x, y, r, p, value, left, right, top, bottom, power;

    int *results = (int *) malloc(sizeof(int) * 6);
    int *status = (int *) malloc(sizeof(int) * T);     // status[i] =  0=>intact | 1=>hit | -1=>killed 
    bool *civil = (bool *) malloc(sizeof(bool) * T);    // civil[i] =   true=>civilian | false=>military

    mt_kill = mt_hit = mt_intact = ct_kill = ct_hit = ct_intact = 0;
    x = 0;
    y = 1;
    r = 2;
    p = 3;
    value = 2;

    // analyze attack i on every target j
    for (i = 0; i < B; i++) {
        left = attacks[i*4 + x] - attacks[i*4 + r];
        right = attacks[i*4 + x] + attacks[i*4 + r];
        top = attacks[i*4 + y] + attacks[i*4 + r];
        bottom = attacks[i*4 + y] - attacks[i*4 + r];
        power = attacks[i*4 + p];
        //printf("Attack[%d] | left=%d right=%d top=%d bottom=%d | power=%d\n", i, left, right, top, bottom, power);

        for (j=0; j<T; j++) {
            // classify military and civilian targets on first round of attacks
            if (i == 0) {
                if (targets[j*3 + value] > 0) civil[j] = true;
                else civil[j] = false;
            }

            /*printf("    Target[%d] | x=%d y=%d | inside=%d | civil=%d | value=%d\n", j, targets[j*3 + x], targets[j*3 + y],
                    inside_square(targets[j*3 + x], targets[j*3 + y], left, right, top, bottom), civil[j], 
                    targets[j*3 + value]);*/

            // if the target is not dead, hit it and see if it dies
            if (!status[j] != -1 && inside_square(targets[j*3 + x], targets[j*3 + y], left, right, top, bottom)){
                // hit the target's value (health points)
                targets[j*3 + value] = hit_target(targets[j*3 + value], power);

                if (targets[j*3 + value] == 0) status[j] = -1;  // kill target j
                else status[j] = 1;                             // hit target j becaue it's not dead yet
            }
        }
    }

    for (i = 0; i < T; i++) {
        switch(status[i]) {
            case 0:
                if (civil[i]) ct_intact++;
                else mt_intact++;
            break;
            case 1:
                if (civil[i]) ct_hit++;
                else mt_hit++;
            break;
            case -1:
                if (civil[i]) ct_kill++;
                else mt_kill++;
            break;
        }
    }

    results[0] = mt_kill;
    results[1] = mt_hit;
    results[2] = mt_intact;
    results[3] = ct_kill;
    results[4] = ct_hit;
    results[5] = ct_intact;
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

int *read_map(int T){
    // our map will be a big array like
    // [x1,y1,value1, x2,y2,value2, x3,y3,value3, ..., xi,yi,valuei]
    int i, x, y, target_value;
    int *war_map = (int *)malloc(sizeof(int) * T * 3);
    assert(war_map != NULL);

    for (i = 0; i < T; i++) {
        scanf("%d %d %d", &x, &y, &target_value);
        war_map[i*3] = x;
        war_map[i*3 + 1] = y;
        war_map[i*3 + 2] = target_value;
    }
    return war_map;
}

int *read_attacks(int B) {
    // our attacks will be an array like
    // [x1,y1,r1,p1, x2,y2,r2,p2, x3,y3,r3,p3, ..., xi,yi,ri,pi]
    int i, x, y, r, p;
    int *attacks = (int *)malloc(sizeof(int) * B * 4);
    assert(attacks != NULL);

    for (i=0; i<B; i++) {
        scanf("%d %d %d %d", &x, &y, &r, &p);
        attacks[i*4] = x;
        attacks[i*4 + 1] = y;
        attacks[i*4 + 2] = r;
        attacks[i*4 + 3] = p;
    }
    return attacks;
}

int main(int argc, char** argv) {
    int world_rank, world_size, N, T, B, i, j;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // read input on the root process
    int *war_map = NULL, *attacks = NULL;

    if (world_rank == 0) {
        scanf("%d", &N);
        scanf("%d", &T);
        war_map = read_map(T);

        /*printf("war_map:\n");
        for (i=0; i<T; i++) {
            printf("%d %d %d\n", war_map[i*3], war_map[i*3+1], war_map[i*3+2]);
        }
        printf("\n");*/

        scanf("%d", &B);
        attacks = read_attacks(B);

        /*printf("attacks:\n");
        for (i=0; i<B; i++) printf("%d %d %d %d\n", attacks[i*4], attacks[i*4+1], attacks[i*4+2], attacks[i*4+3]);
        printf("\n");*/

        // send N, B and attacks to all slave processes
        for (i=1; i<world_size; i++) {
            MPI_Send(&N, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&T, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&B, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(attacks, B*4, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Status status;
        MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&T, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&B, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        attacks = (int *)malloc(sizeof(int) * B * 4);
        MPI_Recv(attacks, B*4, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        /*printf("attacks:\n");
        for (i=0; i<B; i++) printf("%d %d %d %d\n", attacks[i*4], attacks[i*4+1], attacks[i*4+2], attacks[i*4+3]);
        printf("\n");*/
    }
    
    // For each process, create a buffer that will hold a subset of the entire
    // array
    int targets_per_node = T / world_size;
    int segment_length = 3 * targets_per_node;
    int *sub_war_map = (int *)malloc(sizeof(int) * segment_length);
    assert(sub_war_map != NULL);

    // Scatter the targets from the root process to all processes in
    // the MPI world
    MPI_Scatter(war_map, segment_length, MPI_INT, sub_war_map, 
                segment_length, MPI_INT, 0, MPI_COMM_WORLD);

    /*printf("sub_war_map:\n");
    for (i=0; i<targets_per_node; i++) {
        printf("%d %d %d\n", sub_war_map[i*3], sub_war_map[i*3+1], sub_war_map[i*3+2]);
    }
    printf("\n");*/

    // run bombing simulation on your subset
    int *sub_war_result = run_simulation(targets_per_node, sub_war_map, B, attacks);
    /*printf("I am the process %d and my war results are: (%d %d %d %d %d %d)\n",
           world_rank, sub_war_result[0], sub_war_result[1], sub_war_result[2],
           sub_war_result[3], sub_war_result[4], sub_war_result[5]);*/
  
    // gather all partial results down to the root process
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
        printf("Military Targets totally destroyed: %d\n", war_results[0]);
        printf("Military Targets partially destroyed: %d\n", war_results[1]);
        printf("Military Targets not affected: %d\n", war_results[2]);
        printf("Civilian Targets totally destroyed: %d\n", war_results[3]);
        printf("Civilian Targets partially destroyed: %d\n", war_results[4]);
        printf("Civilian Targets not affected: %d\n", war_results[5]);

        // Compute the average across the original data for comparison
        original_results = run_simulation(T, war_map, B, attacks);
        printf("\nREAL RESULTS (NOT PARALLEL):\n\n");
        printf("Military Targets totally destroyed: %d\n", original_results[0]);
        printf("Military Targets partially destroyed: %d\n", original_results[1]);
        printf("Military Targets not affected: %d\n", original_results[2]);
        printf("Civilian Targets totally destroyed: %d\n", original_results[3]);
        printf("Civilian Targets partially destroyed: %d\n", original_results[4]);
        printf("Civilian Targets not affected: %d\n", original_results[5]);
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
