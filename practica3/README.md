1:
    mpiexec --disable-hostname-propagation -f ../maquinas.txt -np 2 ./blocksends 150000000
    Message size = 150000000 floats
    Message size = 150000000 floats
    Task 1 initialized
    Task 0 initialized
    Ready to send messages
    Elapsed time for synchronous send =   911313 uSec
    Elapsed time for ready send =   875236 uSec
    Elapsed time for buffer allocation =       62 uSec
    Elapsed time for buffered send =  5253894 uSec
    Elapsed time for standard send = 10421390 uSec

    We used 150000000 as argument to stress the program and let us see real time
    differences and consequences:
        - Ready send is the fastest because it doesn't guanrantee that the receive has
          been posted, so it leaves those checks to the programmer.
        - Synchronous send comes 2nd place as it only has to wait for a matching receive
          to be posted.
        - Buffered send takes the 3rd place as it has to buffer when it receives lots
          of data.
        - For argument > 400000000, the buffer allocation part gets screwed up and takes
          over 10000000 uSec to fulfill its work, thus making the buffered send take a
          lot longer than the others, even longer than the standard send.
        - Standard send is the slowest as it is waiting for a matching receive and for
          the send buffer to be available.


2:
    We solved the deadlock issue by adding asynchronous communication between processes,
    with the help of MPI_request, MPI_Isend and MPI_Irecv. These allowed each process
    to continue its execution while its message was being delivered to the other one.

3: 
    Results:
        - Elapsed time for send using recv         = 10000045 uSec
        - Elapsed time for send using irecv + wait =       41 uSec

    So, the 'irecv + wait' strategy did 10 seconds faster, mainly because it doesn't 
    wait for the message to be received entirely, it just registers the message to
    be received asynchronously by the system and continues with its work without blocking.