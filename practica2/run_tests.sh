# remove executables to avoid 'stale file' error given by mpirun
rm cuentaSec
rm cuentaPar

# Secuential version

# without optimization
echo "gcc -O$optimization cuentaSec.c -o cuentaSec"
gcc cuentaSec.c -o cuentaSec

## for each problem size
for N in 100000 200000 400000 600000 800000 1000000
do
    echo "./cuentaSec $N"
    ./cuentaSec $N
done
rm cuentaSec

# with optimization
# for each optimization level desired to test
for optimization in 3
do
    echo "gcc -O$optimization cuentaSec.c -o cuentaSec"
    gcc -O$optimization cuentaSec.c -o cuentaSec
    
    # for each problem size
    for N in 100000 200000 400000 600000 800000 1000000
    do
        echo "./cuentaSec $N"
        ./cuentaSec $N
    done
    rm cuentaSec
done

# Parallel version
# for each number of processors
for num_procs in 1 2 4 8 16
do
    echo "mpicc -O3 cuentaPar.c -o cuentaPar"
    mpicc -O3 cuentaPar.c -o cuentaPar

    # for each problem size
    for N in 100000 200000 400000 600000 800000 1000000
    do
        echo "mpirun --disable-hostname-propagation -f ../maquinas.txt -np $num_procs ./cuentaPar $N"
        mpirun --disable-hostname-propagation -f ../maquinas.txt -np $num_procs ./cuentaPar $N
    done
    rm cuentaPar
done
