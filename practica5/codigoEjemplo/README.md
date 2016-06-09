The matrix is represented as an array with N*N positions.
Implemented a modification of the algorithm used in avg.c, extending it's 
functionality to also calculate min and max values of each row array and
return a 3-element array with these results.
After that, these sub arrays are then combined with MPI's Gather into a
large array of size 3*N. Then, this large array is iterated over inside
another function which purpose is to accumulate values from each 3-element
array. In the end, we only have to output the final triplet indicating
(min, max, average) of the whole matrix.


Sample outputs for different values of N:

N = 6
0.232097 0.785933 0.762432 0.920458 0.209735 0.328498 
0.715637 0.270689 0.544372 0.626277 0.263715 0.780901 
0.530922 0.136378 0.410504 0.614552 0.108549 0.307842 
0.050764 0.632159 0.142135 0.739573 0.729196 0.131478 
0.166988 0.055374 0.410431 0.718014 0.010859 0.020963 
0.284897 0.242956 0.806896 0.047329 0.163414 0.016631 
I am the processs 4 and my (min, max, avg) is: (0.010859 0.718014 0.230438)
I am the processs 0 and my (min, max, avg) is: (0.209735 0.920458 0.539859)
I am the processs 1 and my (min, max, avg) is: (0.263715 0.780901 0.533599)
I am the processs 2 and my (min, max, avg) is: (0.108549 0.614552 0.351458)
I am the processs 3 and my (min, max, avg) is: (0.050764 0.739573 0.404218)
I am the processs 5 and my (min, max, avg) is: (0.016631 0.806896 0.260354)
(min, max, avg) of all elements is (0.010859 0.920458 0.386654)
(min, max, avg) computed across original data is (0.010859 0.920458 0.386654)

N = 7
0.459603 0.355735 0.920922 0.320091 0.968483 0.629387 0.795676 
0.088309 0.700754 0.488054 0.908539 0.782975 0.859912 0.943208 
0.043656 0.836434 0.939290 0.084099 0.550087 0.623068 0.399315 
0.591556 0.962898 0.325955 0.231967 0.131375 0.502293 0.909533 
0.975907 0.574354 0.785043 0.435510 0.930089 0.705966 0.755601 
0.898572 0.335353 0.551276 0.986881 0.036107 0.039331 0.895420 
0.819082 0.899243 0.838628 0.862738 0.735677 0.777918 0.946837 
I am the processs 0 and my (min, max, avg) is: (0.320091 0.968483 0.635700)
I am the processs 1 and my (min, max, avg) is: (0.088309 0.943208 0.681679)
I am the processs 2 and my (min, max, avg) is: (0.043656 0.939290 0.496564)
I am the processs 4 and my (min, max, avg) is: (0.435510 0.975907 0.737496)
I am the processs 6 and my (min, max, avg) is: (0.735677 0.946837 0.840017)
I am the processs 3 and my (min, max, avg) is: (0.131375 0.962898 0.522225)
I am the processs 5 and my (min, max, avg) is: (0.036107 0.986881 0.534706)
(min, max, avg) of all elements is (0.036107 0.986881 0.635484)
(min, max, avg) computed across original data is (0.036107 0.986881 0.635484)

N = 10
0.753176 0.704551 0.591397 0.625754 0.179070 0.481051 0.260352 0.763392 0.055731 0.989823 
0.522295 0.804336 0.942985 0.306920 0.496013 0.710647 0.242628 0.759484 0.609742 0.642462 
0.600694 0.869154 0.646668 0.446442 0.160171 0.788152 0.134324 0.622768 0.992842 0.470483 
0.332755 0.746019 0.175035 0.924152 0.371772 0.354105 0.405204 0.632124 0.117497 0.460934 
0.621948 0.639793 0.265271 0.564933 0.946712 0.761284 0.275580 0.189340 0.520767 0.885321 
0.831802 0.121462 0.754475 0.478471 0.567904 0.914646 0.266622 0.702228 0.537414 0.259464 
0.172711 0.870170 0.005483 0.347746 0.794322 0.377255 0.701851 0.199526 0.009380 0.819349 
0.660460 0.631327 0.459141 0.925731 0.196260 0.405853 0.687015 0.471840 0.595193 0.207782 
0.357161 0.426995 0.329244 0.111637 0.905466 0.897148 0.026283 0.172088 0.599376 0.563697 
0.431552 0.772087 0.433867 0.437035 0.119833 0.228189 0.814290 0.821685 0.427715 0.823670 
I am the processs 8 and my (min, max, avg) is: (0.026283 0.905466 0.438909)
I am the processs 9 and my (min, max, avg) is: (0.119833 0.823670 0.530992)
I am the processs 0 and my (min, max, avg) is: (0.055731 0.989823 0.540430)
I am the processs 2 and my (min, max, avg) is: (0.134324 0.992842 0.573170)
I am the processs 1 and my (min, max, avg) is: (0.242628 0.942985 0.603751)
I am the processs 3 and my (min, max, avg) is: (0.117497 0.924152 0.451960)
I am the processs 4 and my (min, max, avg) is: (0.189340 0.946712 0.567095)
I am the processs 6 and my (min, max, avg) is: (0.005483 0.870170 0.429779)
I am the processs 5 and my (min, max, avg) is: (0.121462 0.914646 0.543449)
I am the processs 7 and my (min, max, avg) is: (0.196260 0.925731 0.524060)
(min, max, avg) of all elements is (0.005483 0.992842 0.520360)
(min, max, avg) computed across original data is (0.005483 0.992842 0.520360)


