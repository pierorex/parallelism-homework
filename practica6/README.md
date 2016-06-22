OpenMP Practice

PI calculation using Monte Carlo simulation
    - Results:
        - Without OpenMP
        iterations       error       time (secs.)
           100000      0.000073         0.008
          1000000      0.000071         0.057
         10000000      0.000463         0.489

        - With OpenMP
        iterations       error       time (secs.)
            100000     0.007193         0.067
           1000000     0.001393         0.613
          10000000     0.000459         7.076

        - Parallelization is not the right answer for this problem


Matrix Multiplication:
    - Results:
        - Without OpenMP
        N=800, time (secs.) = 7.558

        - With OpenMP
        threads   N    time (secs.)
           1     800      5.049
           2     800      3.661
           3     800     10.585
           4     800     14.559

    - As we can see, just by taking away the accumulation in an array position
      and doing it in a 'result' variable gives us an improvement 
      of ~2.5 seconds.
    - By increasing the number of threads to 2, we improved ~1.4 seconds, due
      to the utilization of both cores in my machine.
    - While we increase the number of threads used above 2, OpenMP can't help 
      us any further because the system only has 2 cores, thus any extra 
      threads will be forcing cores sharing, which inevitably causes 
      bottlenecks.
    - Parallelization is a good fit for this problem as we can run expensive
      operations in different threads.