build:
	mpicc MPI_Json.c -o MPI_Json -ljson-c

run:
	mpiexec -np 2 ./MPI_Json

all:
	mpicc MPI_Json.c -o MPI_Json -ljson-c
	mpiexec -np 2 ./MPI_Json
