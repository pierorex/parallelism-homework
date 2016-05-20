Secuential executions
Cardinality | w/o optimization | w/ optimization O3 |
  100.000            5:944             1:463
  200.000           12:152             3:184
  400.000           24:621             7:358
  600.000           36:942            11:438
  800.000           49:509            15:396
1.000.000           62:15             19:462

Optimized runs with -O3 get results a lot faster than those without optimization

Parallel executions
Cardinality | P = 2  |  P = 4  |  P = 8  |  P = 16  |  P = 1  |
  100.000      0:749     0:384     0:198     0:121      1:490
  200.000      1:514     0:772     0:405     0:192      3:785
  400.000      3:640     1:551     0:778     0:423     10:269
  600.000      7:189     2:257     1:150     0:649     16:226
  800.000     11:427     3:646     1:556     0:794     21:737
1.000.000     14:320     5:509     1:922     0:943     27:750

Last column (P=1) is used to calculate acceleration.
Analysis:
- For small instances, differences between number of processors doesn't seem very interesting
  because sequential and communication parts of the program use almost all the time spent
- For big instances, huge differences can be seen because of heavy computational tasks sent 
  to each process can be executed alongside each other. This is usual in parallel programs,
  using small instances of a problem usually give lesser speedups than those obtained when
  comparing executions with big and massive instances of the same problem
- Each processor gets an array of size N/P (N=size of original array, P=Number of processors),
  so its computing time is a fraction of the original problem
- Because sub-arrays get smaller while we increment P, the communication time per process 
  gets lower but the total ammount of communications gets bigger


Acc(n) = Time with P=1 / Time with P=n

Acceleration (for parallel version only)
Cardinality |  P = 2  |  P = 4  |  P = 8  |  P = 16  |
  100.000      1.989     3.880     7.525     12.314
  200.000      2.500     4.902     9.345     19.713 
  400.000      2.821     6.620    13.199     23.770
  600.000      2.257     7.189     9.014     25.001
  800.000      1.902     5.961    18.901     27.376
1.000.000      1.937     5.037    17.834     29.427

Analysis:
- As expected, acceleration is really high because of the massive speedups we got from the
  parallel execution chart


Eff = Acc / P

Efficiency (for parallel version only)
Cardinality |  P = 2  |  P = 4  |  P = 8  |  P = 16  |
  100.000      0.994     1.940     3.762     6.157
  200.000      1.250     2.451     4.672     9.856
  400.000      1.410     3.310     6.599    11.885
  600.000      1.128     3.594     4.507    12.500
  800.000      0.951     2.980     9.450    13.688
1.000.000      0.968     2.518     8.917    14.713

Analysis:
- We get very high efficiencies because the acceleration obtained was, in many cases, 
  higher than the number of processors used