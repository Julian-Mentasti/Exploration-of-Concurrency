# Week 3 Number sieve with MPI

To compile:

```
mpicc sieve-mpi.c -o sieve-mpi
```

To run:
```
mpiexec -np n ./sieve-mpi
```

where n is the number of primes you want.

It self terminates, although the termination time increases exponentially with N but it will die
eventually...
