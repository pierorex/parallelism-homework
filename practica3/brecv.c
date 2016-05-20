/* -----------------------------------------------------------------------
 * Code:   brecv.c
 * Lab:    MPI Point-to-Point Communication
 *         This program demonstrates that replacing a blocking receive
 *         with a non-blocking receive earlier in the program can decrease
 *         the synchronization time on the corresponding send.
 * Usage:  brecv
 *         Run on two nodes
 * Author: Roslyn Leibensperger  Last revised: 8/30/95 RYL
 * ------------------------------------------------------------------------ */
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include "mpi.h"
#define MSGLEN 40480
#define TAG 100
void print_time ( struct timeval tbegin, struct timeval tend );

main( argc, argv ) 
     int argc;
     char **argv;
{
  float message [MSGLEN];    /* message buffer               */
  int rank,                  /* rank of task in communicator */
      i;
  MPI_Status status;         /* status of communication      */
  MPI_Request request;
  struct timeval tbegin,     /* used to measure elapsed time */
                 tend;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  printf ("Process %d initialized\n", rank);

  /* -----------------------------------------------------------------
   * task 0 will report the elapsed time for a blocking send
   * ----------------------------------------------------------------- */
  if (rank == 0)  {
    for (i=0; i<MSGLEN; i++) message[i] = 100;
    printf ("Task %d sending message\n", rank );
    gettimeofday ( &tbegin, (struct timezone*)0 );
    MPI_Ssend ( message, MSGLEN, MPI_FLOAT, 1, TAG, MPI_COMM_WORLD );
    gettimeofday ( &tend, (struct timezone*)0 );   
    print_time ( tbegin, tend );
  }

  /* -----------------------------------------------------------------
   * task 1 sleeps for 10 seconds, and then calls a blocking receive.
   * the sleep is intended to simulate time spent in useful computation
   * ----------------------------------------------------------------- */
  else if (rank == 1)  {
    for (i=0; i<MSGLEN; i++) message[i] = -100;
    MPI_Irecv (message, MSGLEN, MPI_FLOAT, 0, TAG, MPI_COMM_WORLD, &request);
    sleep(10);
    //MPI_Recv ( message, MSGLEN, MPI_FLOAT, 0, TAG, MPI_COMM_WORLD, &status);
    MPI_Wait(&request, &status);
    printf ("Task %d received message\n", rank );
  }
  MPI_Finalize ();
  return 0;
}

/* -------------------------------------------------------------------
 * calculate elapsed time and print results
 * ------------------------------------------------------------------- */
void print_time (struct timeval tbegin, struct timeval tend) {
  int dt;
  dt = (tend.tv_sec - tbegin.tv_sec) * 1000000 + tend.tv_usec 
    - tbegin.tv_usec;
  printf ("Elapsed time for send = %8d uSec\n", dt);
}
