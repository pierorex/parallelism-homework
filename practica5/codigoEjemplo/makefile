EXECS=avg all_avg matrix_avg
MPICC?=mpicc

all: ${EXECS}

avg: avg.c
	${MPICC} -o avg avg.c

all_avg: all_avg.c
	${MPICC} -o all_avg all_avg.c

matrix_avg: matrix_avg.c
	${MPICC} -o matrix_avg matrix_avg.c

clean:
	rm -f ${EXECS}
