#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>


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
    int i, j, mt_kill, mt_hit, mt_intact, ct_kill, ct_hit, ct_intact,
        value, left, right, top, bottom, x, y, r, p, power;

    int *results = (int *) malloc(sizeof (int) * 6);
    int *status  = (int *) malloc(sizeof (int) * T);  // status[i] =  0=>intact | 1=>hit | -1=>killed 
    bool *civil = (bool *) malloc(sizeof(bool) * T);  // civil[i] =   true=>civilian | false=>military
    /*printf("T=%d\ntargets = ", T);
    for (i=0; i<3*T; i++)
        printf("%d ", targets[i]);
    printf("\n");*/

    for(i = 0; i < T; i++) {
        status[i] = 0;          // all targets are 'intact'
        civil[i] = false;       // all targets are military (we turn them into civilian as soon as we scan them)
    }

    // shortcuts to find attack's parameters (e.g. x coordinate is attacks[i*4 + x], etc)
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
            if (i == 0) {
                // classify targets as military or civilian on the first round of attacks
                if (targets[j*3 + value] > 0) civil[j] = true;
                else civil[j] = false;
            }

            /*printf("    Target[%d] | x=%d y=%d | inside=%d | civil=%d | value=%d | status=%d\n", j, targets[j*3 + x], targets[j*3 + y],
                    inside_square(targets[j*3 + x], targets[j*3 + y], left, right, top, bottom), civil[j], 
                    targets[j*3 + value], status[j]);*/

            if (status[j] != -1 && inside_square(targets[j*3 + x], targets[j*3 + y], left, right, top, bottom)){
                // target is not dead but is within attack's radius

                // hit the target's value (health points)
                targets[j*3 + value] = hit_target(targets[j*3 + value], power);

                if (targets[j*3 + value] == 0) status[j] = -1;  // kill target j if health points == 0
                else status[j] = 1;                             // hit target j becaue it's not dead yet
                //printf("    status=%d\n", status[j]);
            }
        }
    }

    mt_kill = mt_hit = mt_intact = ct_kill = ct_hit = ct_intact = 0;

    // count how many got hit, killed or stayed intact
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

    // build a results array to be returned
    results[0] = mt_kill;
    results[1] = mt_hit;
    results[2] = mt_intact;
    results[3] = ct_kill;
    results[4] = ct_hit;
    results[5] = ct_intact;
    return results;
}


void run_sequential(int T, int *targets, int B, int *attacks) {
    // debugging function built to compare results against the parallel version
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
    // accumulate results arrays from many simulations into a single one
    // containing the sum of all simulations' attributes
    int *results = (int *) malloc(sizeof(int) * 6);

    int i;
    for (i=0; i < 6; i++) results[i] = 0;

    for (i = 0; i < num_elements; i+=6) {
        results[0] += array[i];     // mt_kill
        results[1] += array[i+1];   // mt_hit
        results[2] += array[i+2];   // mt_intact
        results[3] += array[i+3];   // ct_kill
        results[4] += array[i+4];   // ct_hit
        results[5] += array[i+5];   // ct_intact
    }
    return results;
}


int *read_targets(int T){
    // our targets will be in a big array like
    // [x1,y1,value1, x2,y2,value2, x3,y3,value3, ..., xi,yi,valuei]
    // x, y := target's coordinate on a 2D plane
    // value := target's health points
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
    // our attacks will be in a big array like
    // [x1,y1,r1,p1, x2,y2,r2,p2, x3,y3,r3,p3, ..., xi,yi,ri,pi]
    // x, y := attacks coordinates on a 2D plane
    // r := radius of attack. Using (x,y) and r we can build a squared area of damage
    // p := power of the attack
    int i, x, y, radius, power;
    int *attacks = (int *) malloc(sizeof(int) * B * 4);
    assert(attacks != NULL);

    for (i=0; i<B; i++) {
        scanf("%d %d %d %d", &x, &y, &radius, &power);
        attacks[i*4] = x;
        attacks[i*4 + 1] = y;
        attacks[i*4 + 2] = radius;
        attacks[i*4 + 3] = power;
    }
    return attacks;
}

int main(int argc, char** argv) {
    int world_rank, world_size, N, T, B, i, j;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int *targets = NULL, *attacks = NULL;

    if (world_rank == 0) {
        // read input on the root process
        scanf("%d", &N);
        scanf("%d", &T);
        targets = read_targets(T);

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
    int extra_targets = 0;

    // handle special case when number of processes (world_size) is not a
    // multiple of the number of targets (T). In this case we add some
    // space for extra targets in the master node
    if (world_rank == 0 && T % world_size != 0)
        extra_targets = T % world_size;

    int *sub_targets = (int *) malloc(sizeof(int) * (segment_length + 3*extra_targets));
    assert(sub_targets != NULL);

    // Scatter the targets from the root process to all processes in
    // the MPI world
    MPI_Scatter(targets, segment_length, MPI_INT, sub_targets, 
                segment_length, MPI_INT, 0, MPI_COMM_WORLD);

    if (world_rank == 0 && extra_targets != 0) {
        // we added space for extra targets in master, now we add those targets
        for (i = 0; i < 3*extra_targets; i++)
            sub_targets[segment_length + i] = targets[segment_length*world_size + i];
        /*printf("extra_targets=%d init_extra=%d end_extra=%d\n", extra_targets, 
                segment_length*world_size, segment_length*world_size + 3*extra_targets);
        printf("master sub_targets = ");
        for (i=0; i<segment_length+3*extra_targets; i++) printf("%d ", sub_targets[i]);
        printf("\n");*/
    }

    /*printf("\ntargets_per_node=%d segment_length=%d\nsub_targets = ", targets_per_node, segment_length);
    for(i=0; i<segment_length; i++){
        printf("%d ", sub_targets[i]);
    }
    printf("\n");*//*
    printf("B=%d\nattacks = ", B);
    for(i=0; i<B*4; i++){
        printf("%d ", attacks[i]);
    }
    printf("\n");*/

    // run bombing simulation on your subset
    int *sub_war_result = run_simulation(targets_per_node + extra_targets,
                                         sub_targets, B, attacks);
    /*printf("%d %d %d %d %d %d\n", sub_war_result[0], sub_war_result[1],
        sub_war_result[2], sub_war_result[3], sub_war_result[4], sub_war_result[5]);*/
  
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
