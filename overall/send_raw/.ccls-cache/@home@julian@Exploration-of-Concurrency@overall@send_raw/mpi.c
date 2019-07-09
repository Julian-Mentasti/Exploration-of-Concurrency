#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "rellist.h"

int rank;
int size;

// This thread will send data to the speaker
void reader(int rank, int size)
{
    printf("reader at %d\n", rank);
    int* outbox = calloc(1, sizeof(int));
    *outbox = 1;
    MPI_Send(outbox, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    free(outbox);
    return;
}

// This thread will print the data from the reader.
void speaker(int rank, int size)
{
    printf("speaker at %d\n", rank);
    int* inbox = calloc(1, sizeof(int));
    MPI_Recv(inbox, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("I got %d\n", *inbox);
    free(inbox);
    return;
}

int main(int argc, char** argv)
{
    MPI_Init(NULL,NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank) {
        speaker(rank, size);
    } else {
        reader(rank, size);
    }

    MPI_Finalize();
    return 0;
}
