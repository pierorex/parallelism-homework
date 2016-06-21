For Bombing Fields, we divided the domain by targets, so every process will
handle a different subset of the targets. The master node will handle the
"excess" that might appear during the targets set partitioning.
Each process will be in charge of running the bombing simulation over its 
targets to tell us their status when it finishes, so we can then sum those
results and get the overall status of the battle field.

Compiling:
mpicc bombing_fields.c -o bombing_fields

Executing:
mpirun -n 2 -f machilefile.txt ./bombing_fields < input_file

The -n parameter tells MPI how many processes to use, the -f parameter is used
to determine the machines file to be used.