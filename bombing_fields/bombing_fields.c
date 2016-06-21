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

    for(i = 0; i < T; i++) {
        status[i] = 0;
        civil[i] = false;
    }

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

            /*printf("    Target[%d] | x=%d y=%d | inside=%d | civil=%d | value=%d | status=%d\n", j, targets[j*3 + x], targets[j*3 + y],
                    inside_square(targets[j*3 + x], targets[j*3 + y], left, right, top, bottom), civil[j], 
                    targets[j*3 + value], status[j]);*/

            // if the target is not dead, hit it and see if it dies
            if (status[j] != -1 && inside_square(targets[j*3 + x], targets[j*3 + y], left, right, top, bottom)){
                // hit the target's value (health points)
                targets[j*3 + value] = hit_target(targets[j*3 + value], power);

                if (targets[j*3 + value] == 0) status[j] = -1;  // kill target j
                else status[j] = 1;                             // hit target j becaue it's not dead yet
                //printf("    status=%d\n", status[j]);
            }
        }
    }

    mt_kill = mt_hit = mt_intact = ct_kill = ct_hit = ct_intact = 0;

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


void run_sequential(int T, int *targets, int B, int *attacks) {
    int *original_results = run_simulation(T, targets, B, attacks);
    printf("\nREAL RESULTS (NOT PARALLEL):\n\n");
    printf("Military Targets totally destroyed: %d\n", original_results[0]);
    printf("Military Targets partially destroyed: %d\n", original_results[1]);
    printf("Military Targets not affected: %d\n", original_results[2]);
    printf("Civilian Targets totally destroyed: %d\n", original_results[3]);
    printf("Civilian Targets partially destroyed: %d\n", original_results[4]);
    printf("Civilian Targets not affected: %d\n", original_results[5]);
    free(original_results);
}


int *accumulate_simulations(int *array, int num_elements) {
    int *results = (int *) malloc(sizeof(int) * 6);

    int i;
    for (i=0; i < 6; i++) results[i] = 0;

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
    int *targets = (int *) malloc(sizeof(int) * T * 3);
    assert(targets != NULL);

    for (i = 0; i < T; i++) {
        scanf("%d %d %d", &x, &y, &target_value);
        targets[i*3] = x;
        targets[i*3 + 1] = y;
        targets[i*3 + 2] = target_value;
    }
    return targets;
}

int *read_attacks(int B) {
    // our attacks will be an array like
    // [x1,y1,r1,p1, x2,y2,r2,p2, x3,y3,r3,p3, ..., xi,yi,ri,pi]
    int i, x, y, r, p;
    int *attacks = (int *) malloc(sizeof(int) * B * 4);
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
    int *targets = NULL, *attacks = NULL;

    if (world_rank == 0) {
        scanf("%d", &N);
        scanf("%d", &T);
        targets = read_map(T);

        scanf("%d", &B);
        attacks = read_attacks(B);

        // send N, T, B and attacks to all slave processes
        for (i=1; i<world_size; i++) {
            MPI_Send(&N, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&T, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&B, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(attacks, B*4, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    else {
        // receive N, T, B and attacks from master
        MPI_Status status;
        MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&T, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&B, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        attacks = (int *) malloc(sizeof(int) * B * 4);
        MPI_Recv(attacks, B*4, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }
    
    // For each process, create a buffer that will hold a subset of the entire
    // array
    int targets_per_node = T / world_size;
    int segment_length = 3 * targets_per_node;
    int *sub_targets = (int *) malloc(sizeof(int) * segment_length);
    assert(sub_targets != NULL);

    // Scatter the targets from the root process to all processes in
    // the MPI world
    MPI_Scatter(targets, segment_length, MPI_INT, sub_targets, 
                segment_length, MPI_INT, 0, MPI_COMM_WORLD);

    printf("targets_per_node=%d segment_length=%d\nsub_targets = ", targets_per_node, segment_length);
    for(i=0; i<segment_length; i++){
        printf("%d ", sub_targets[i]);
    }
    printf("\n");
    printf("B=%d\nattacks = ", B);
    for(i=0; i<B*4; i++){
        printf("%d ", attacks[i]);
    }
    printf("\n");

    // run bombing simulation on your subset
    int *sub_war_result = run_simulation(targets_per_node, sub_targets, B, attacks);
    printf("%d %d %d %d %d %d\n", sub_war_result[0], sub_war_result[1],
        sub_war_result[2], sub_war_result[3], sub_war_result[4], sub_war_result[5]);
  
    // gather all partial results down to the root process
    int *sub_war_results = NULL;
    if (world_rank == 0) {
        sub_war_results = (int *) malloc(sizeof(int) * 6 * world_size);
        assert(sub_war_results != NULL);
    }
    MPI_Gather(sub_war_result, 6, MPI_INT, sub_war_results, 6, MPI_INT, 0,
               MPI_COMM_WORLD);

    int *war_results;
    if (world_rank == 0) {
        war_results = accumulate_simulations(sub_war_results, 6 * world_size);
        printf("Military Targets totally destroyed: %d\n", war_results[0]);
        printf("Military Targets partially destroyed: %d\n", war_results[1]);
        printf("Military Targets not affected: %d\n", war_results[2]);
        printf("Civilian Targets totally destroyed: %d\n", war_results[3]);
        printf("Civilian Targets partially destroyed: %d\n", war_results[4]);
        printf("Civilian Targets not affected: %d\n", war_results[5]);

        // Compute results across the original data for comparison
        run_sequential(T, targets, B, attacks);
    }

    // Clean up
    if (world_rank == 0) {
        free(sub_war_results);
        free(war_results);
    }
    free(attacks);
    free(targets);
    free(sub_targets);
    free(sub_war_result);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
}
