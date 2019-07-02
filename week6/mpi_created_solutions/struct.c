#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

void main(int argc, char *argv[])
{

    MPI_Init(&argc, &argv);
    int size, rank;
    int m,n;
    m=n=2;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    typedef struct estruct
    {
        float *array;
        int sizeM, sizeK, sizeN, rank_or;
    } ;

    struct estruct kernel, server;

    MPI_Datatype types[5] = {MPI_FLOAT, MPI_INT,MPI_INT,MPI_INT,MPI_INT};
    MPI_Datatype newtype;
    int lengths[5] = {n*m,1,1,1,1};
    MPI_Aint displacements[5];
    displacements[0] = (size_t) & (kernel.array[0]) - (size_t)&kernel;
    displacements[1] = (size_t) & (kernel.sizeM) - (size_t)&kernel;
    displacements[2] = (size_t) & (kernel.sizeK) - (size_t)&kernel;
    displacements[3] = (size_t) & (kernel.sizeN) - (size_t)&kernel;
    displacements[4] = (size_t) & (kernel.rank_or) - (size_t)&kernel;


    MPI_Type_struct(5, lengths, displacements, types, &newtype);
    MPI_Type_commit(&newtype);

    if (rank == 0)
    {
        kernel.array  = (float *)malloc(m * n * sizeof(float));
        for(int i = 0; i < m*n; i++) kernel.array[i] = i;
        kernel.sizeM = 5;
        kernel.sizeK = 5;
        kernel.sizeN = 5;
        kernel.rank_or = 5;
        MPI_Send(&kernel, 1, newtype, 1, 0, MPI_COMM_WORLD);
    }
    else
    {
        server.array  = (float *)malloc(m * n * sizeof(float));
        MPI_Recv(&server, 1, newtype, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("%i \n", server.sizeM);
        printf("%i \n", server.sizeK);
        printf("%i \n", server.sizeN);
        printf("%i \n", server.rank_or);
        for(int i = 0; i < m*n; i++) printf("%f\n",server.array[i]);
    }

    MPI_Finalize();
}
