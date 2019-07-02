#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mpi.h"

typedef struct{
    float x;
    float y;
    int centroid;
} point;

typedef struct{
    int csize;//the current size
    int tsize;//the total size
    point * data;//the data carried
} ArrayList;


int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    assert( size >= 2 );
    const int npts_avail=20;
    point points[npts_avail];

    ArrayList list;
    list.data = points;

    if (rank == 0) {
        int npts_used=10;
        list.csize = npts_used;
        list.tsize = npts_avail;
        for (int i=0; i<list.csize; i++) {
            points[i].x = 1.*i;
            points[i].y = -2.*i;
            points[i].centroid = i;
        }
    }

    const int nfields=3;
    MPI_Aint disps[nfields];
    int blocklens[] = {1,1,1};
    MPI_Datatype types[] = {MPI_FLOAT, MPI_FLOAT, MPI_INT};

    disps[0] = offsetof( point, x );
    disps[1] = offsetof( point, y );
    disps[2] = offsetof( point, centroid );

    MPI_Datatype istruct, pstruct;
    MPI_Type_create_struct(nfields, blocklens, disps, types, &istruct );
    MPI_Type_create_resized( istruct, 0, (char *)&(points[1]) - (char *)(&points[0]), &pstruct );
    MPI_Type_commit(&pstruct);

    if (rank == 0) {
        MPI_Send( &(list.csize), 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send( &(list.tsize), 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send( list.data, list.csize, pstruct, 1, 0, MPI_COMM_WORLD);
    } else if (rank == 1) {
        MPI_Recv( &(list.csize), 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv( &(list.tsize), 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv( list.data, list.csize, pstruct, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    if (rank == 1) {
        printf("Received: \n");
        for (int i=0; i<list.csize; i++) {
            printf(" (%f, %f): %d\n", points[i].x, points[i].y, points[i].centroid);
        }
    }

    MPI_Finalize();
}
