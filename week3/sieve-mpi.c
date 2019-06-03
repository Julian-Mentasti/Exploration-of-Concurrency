#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// Constants
int rank;
int size;


void generator() {

    int *inbox = calloc(1, sizeof(int));
    int *outbox = calloc(1, sizeof(int));

    for (int i = 2; ; i++) {
        *outbox = i;
        // Send the number
        MPI_Send(outbox, 1, MPI_INT, (rank+1), 0, MPI_COMM_WORLD);
        // Recieve the death sinal
        MPI_Recv(inbox, 1, MPI_INT, (rank+1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // Check the death sinal
        if (*inbox == -1) {
            free(inbox);
            free(outbox);
            return; // die
        }
    }
}

void worker(int rank, int size)
{
    int myPrime = 0;
    int told = 0;
    int *inbox = calloc(1, sizeof(int));
    // printf("Greetings from worker %d\n", rank);
    int *outbox = calloc(1, sizeof(int));

    *outbox = 1;
    MPI_Send(outbox, 1, MPI_INT, (rank-1), 0, MPI_COMM_WORLD);

    while (1) {

        // recieve number from previous
        MPI_Recv(inbox, 1, MPI_INT, (rank-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // printf("First recv %d\n", rank);

        // Assign a prime
        if (!myPrime) {
            myPrime = *inbox;
            if (!told) {
                // tell the world
                printf("Found a prime: %d\n", myPrime);
                told = 1;
            }
        }

        if (*inbox%myPrime) {
            // if this number is not divisible by myPrime
            // Send to the next filter
            if (!(rank == (size - 1)))
                MPI_Send(inbox, 1, MPI_INT, (rank+1), 0, MPI_COMM_WORLD);
        }
        MPI_Send(inbox, 1, MPI_INT, (rank-1), 0, MPI_COMM_WORLD);

        // Recieve death signal

        if (!(rank == (size - 1))) {
            MPI_Recv(inbox, 1, MPI_INT, (rank+1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            // printf("Recieved the death signal %d, rank %d\n", inbox, rank);
            // Propagate
            MPI_Send(inbox, 1, MPI_INT, (rank-1), 0, MPI_COMM_WORLD);

            if (*inbox == -1) {
                free(inbox);
                free(outbox);
                return; // Death
            }
        }

        if (rank == (size - 1)) {
            // Guess ill die
            *outbox = -1;
            // Send the death signal
            MPI_Send(outbox, 1, MPI_INT, (rank-1), 0, MPI_COMM_WORLD);
            free(inbox);
            free(outbox);
            printf("PANIC\n");
            return;
        }

    }
}

int main(int argc, char** argv)
{
    // Initialise the environment
    MPI_Init(NULL, NULL);
    // Get the rank of this process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // Get the size of the world
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Here is where I would do some checking
    // <.<

    if (rank) // If its a worker thread
        worker(rank, size); // Long live the proletariat
    else
        generator(); // Give out numbers

    // End the environment
    MPI_Finalize();
    return 0;
}
