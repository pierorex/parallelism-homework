hola.c:
    - Initially, copy/pasting code from a PDF into nano produced some weird
      errors while compiling the program, this was solved by typing text
      manually
    - hola.c is a pretty simple program where we set a MPI environment and then
      send a message from every slave node to the master node, with the purpose
      of testing send and receive usibg MPI
    - by executing: mpirun --disable-hostname-propagation -np 4 
                    --machinefile ../maquinas.txt ./hola/hola
      we got as output:
        Maestro ejecutandose en nodo04
        Del proceso 1: Hola, desde nodo04
        Del proceso 2: Hola, desde nodo05
        Del proceso 3: Hola, desde nodo05
        Tiempo del Proceso[1] = 6:0 (seg:mseg)
        Tiempo del Proceso[2] = 7:0 (seg:mseg)
        Tiempo del Proceso[3] = 8:0 (seg:mseg)

      which means that everything worked out ok, as every slave process sent
      its message, did its "work", which essentially was sleeping for 
        (5 + process_rank) seconds

cpilog.c:
    - This is a sample program to check MPI profiling capabilities.
    - MPE is really basic regarding usability, one have to set "log points"
      to indicate the system where one want to measure execution time
    - As we can see in the jumpshot screenshots inside the cpilog folder
      (cpilog/mpe-log.png and cpilog/mpe-log-legend.png):
        - Yellow zones represent MPI barriers set in the code to synchronize
          the execution of all processes
        - Dark blue zones represent Compute time
        - Purple zones are Reduce time, process 1 almost always gets longer 
          time in this zone because it has to accumulate values
        - Fast machines have longer Barrier time because they have to wait for
          the others' Broadcasts to occur and get to synchronization point.
    - MPE is a great tool to identify bottlenecks and test different solutions
      for the ones found.