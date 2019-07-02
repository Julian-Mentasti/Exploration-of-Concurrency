#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rellist.h"

int rank;
int size;
int BUFFER_SIZE = 1024;
int limit = 10;
int item_size = sizeof(int);

// This thread will send data to the speaker
void reader(int rank, int size)
{
    printf("reader at %d\n", rank);
    limit = 10;
    int offset = 0;
    int* datafield = calloc(limit, item_size);
    int* datafieldlimit = dataFieldLimit(datafield, item_size, limit);

    int temp = insertFirstRelative(datafield, 1, item_size,offset,datafieldlimit);
    if (temp < 0) exit(1);
    offset = temp;

    temp = insertFirstRelative(datafield, 3, item_size,offset,datafieldlimit);
    if (temp < 0) exit(1);
    offset = temp;

    temp = insertFirstRelative(datafield, 2, item_size,offset,datafieldlimit);
    if (temp < 0) exit(1);
    offset = temp;

    char* buffer = calloc(limit, sizeof(char));
    int res = liftData(buffer, datafield, item_size, offset, 0);
    if (res < 0) exit(1);

    printRelative(datafield, item_size, offset);

    MPI_Send(buffer, 10, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
    free(buffer);
    return;
}

// This thread will print the data from the reader.
void speaker(int rank, int size)
{
    int goffset = 12;
    printf("speaker at %d\n", rank);
    char* inbox = calloc(limit, sizeof(char));
    MPI_Recv(inbox, 10, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    int* datafield = calloc(limit, item_size);
    int res = lowerData(datafield, goffset, inbox, item_size);
    if (res < 0) exit(1);

    printRelative(datafield, item_size, goffset);
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
