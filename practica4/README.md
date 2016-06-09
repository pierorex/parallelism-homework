In these 6 exercises, we showed a way to create different kinds of MPI datatypes
used to easily send data to processes without making lots of send/recv calls.

1:
    - Datatype with IMAX blocks of 1 element and a stride of JMAX
2:
    - Datatype with IMAX blocks of 2 continuous elements and a stride of JMAX
3:
    - Datatype with IMAX blocks of 1 element and a stride of JMAX + 1
4:
    - Datatype with IMAX blocks of 1 element and a stride of JMAX - 1
5:
    - Datatype with JMAX / 2 blocks of 1 element and a stride of 2
    - We send even-numbered rows with a displacement of 0 (&matrix[i][0])
      and odd-numbered rows with a displacement of 1 (&matrix[i][1])
6:
    - Datatype with IMAX*JMAX / 3 + 1 blocks of 1 element and a stride of 3.
      "IMAX*JMAX / 3 + 1" is used to to know how many segments of 3 continuous
      elements we find in the matrix