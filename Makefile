all: integrate omp mpi

integrate:
	gcc -Wall integrate.c -o integrate -lm
omp:
	gcc -Wall -fopenmp integrate_omp.c -o omp -lm
mpi:
	mpicc -Wall integrate_mpi.c -o mpi -lm
clean:
	rm integrate omp mpi