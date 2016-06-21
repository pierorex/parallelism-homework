// Author: Wes Kendall
// Copyright 2012 www.mpitutorial.com
// This code is provided freely with the tutorials on mpitutorial.com. Feel
// free to modify it for your own use. Any distribution of the code must
// either provide a link to www.mpitutorial.com or keep this header intact.
//
// Program that computes the average of an array of elements in parallel using
// MPI_Scatter and MPI_Gather
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <assert.h>

// Creates an array of random numbers. Each number has a value from 0 - 1
float *create_rand_nums(int num_elements) {
  float *rand_nums = (float *)malloc(sizeof(float) * num_elements);
  assert(rand_nums != NULL);
  int i;
  for (i = 0; i < num_elements; i++) {
    rand_nums[i] = (rand() / (float)RAND_MAX);
  }
  return rand_nums;
}

// Computes the average of an array of numbers
float* compute_avg(float *array, int num_elements) {
  float* triplet = (float *)malloc(sizeof(float) * 3);
  float sum = 0.0, min = 1000000.0, max = -1000000.0;

  int i;
  for (i = 0; i < num_elements; i++) {
    sum += array[i];
    if (array[i] > max) max = array[i];
    if (array[i] < min) min = array[i];
  }
  //printf("avg computed");
  triplet[0] = min;
  triplet[1] = max;
  triplet[2] = sum / num_elements;
  return triplet;
}

float* compute_avg_end(float *array, int num_elements) {
  float* triplet = (float *)malloc(sizeof(float) * 3);
  float sum = 0.0, min = 1000000.0, max = -1000000.0;

  int i;
  for (i = 0; i < num_elements; i+=3) {
    // printf("%f %f %f\n", array[i], array[i+1], array[i+2]);
    sum += array[i+2];
    if (array[i+1] > max) max = array[i+1];
    if (array[i] < min) min = array[i];
  }
  triplet[0] = min;
  triplet[1] = max;
  triplet[2] = sum / (num_elements / 3.0);
  return triplet;
}

int main(int argc, char** argv) {
  // Seed the random number generator to get different results each time
  srand(time(NULL));

  MPI_Init(NULL, NULL);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  // Create a random array of elements on the root process. Its total
  // size will be the squared number of processes
  float *rand_nums = NULL;
  if (world_rank == 0) {
    rand_nums = create_rand_nums(world_size * world_size);
  
    int i, j;
    for (i=0; i<world_size; i++) {
      for (j=0; j<world_size; j++) {
        printf("%f ", rand_nums[i*world_size+j]);
      }
      printf("\n");
    }
  }

  // For each process, create a buffer that will hold a subset of the entire
  // array
  float *sub_rand_nums = (float *)malloc(sizeof(float) * world_size);
  assert(sub_rand_nums != NULL);

  // Scatter the random numbers from the root process to all processes in
  // the MPI world
  MPI_Scatter(rand_nums, world_size, MPI_FLOAT, sub_rand_nums,
              world_size, MPI_FLOAT, 0, MPI_COMM_WORLD);
  //printf("data scattered");
  // Compute the average of your subset
  float* sub_avg = compute_avg(sub_rand_nums, world_size);
  printf("I am the processs %d and my (min, max, avg) is: (%f %f %f)\n",
         world_rank, sub_avg[0], sub_avg[1], sub_avg[2]);
  
  // Gather all partial averages down to the root process
  float *sub_avgs = NULL;
  if (world_rank == 0) {
    sub_avgs = (float *)malloc(sizeof(float) * 3 * world_size);
    assert(sub_avgs != NULL);
  }
  MPI_Gather(sub_avg, 3, MPI_FLOAT, sub_avgs, 3, MPI_FLOAT, 0, MPI_COMM_WORLD);
  //if (world_rank == 0) {
  //    printf("gathered: %f %f %f | %f %f %f\n", 
  //            sub_avgs[0], sub_avgs[1], sub_avgs[2],
  //            sub_avgs[3], sub_avgs[4], sub_avgs[5]);
  //}

  // Now that we have all of the partial averages on the root, compute the
  // total average of all numbers. Since we are assuming each process computed
  // an average across an equal amount of elements, this computation will
  // produce the correct answer.
  if (world_rank == 0) {
    float* avg = compute_avg_end(sub_avgs, 3 * world_size);
    printf("(min, max, avg) of all elements is (%f %f %f)\n", avg[0], avg[1], avg[2]);
    // Compute the average across the original data for comparison
    float* original_data_avg =
      compute_avg(rand_nums, world_size * world_size);
    printf("(min, max, avg) computed across original data is (%f %f %f)\n", 
           original_data_avg[0], original_data_avg[1], original_data_avg[2]);
  }

  // Clean up
  if (world_rank == 0) {
    free(rand_nums);
    free(sub_avgs);
  }
  free(sub_rand_nums);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
