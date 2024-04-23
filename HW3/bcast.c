#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define ROOT 0

int main(int argc, char *argv[]) {
    int myid, numprocs;
	int namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

    int n = 0;
	if(myid == 0){
		n = 12345;
	}

    if (myid == ROOT) {
        if (myid * 2 + 1 < numprocs) {
            MPI_Send(&n, 1, MPI_INT, myid * 2 + 1, 0, MPI_COMM_WORLD);
        }
        if (myid * 2 + 2 < numprocs) {
        	MPI_Send(&n, 1, MPI_INT, myid * 2 + 2, 0, MPI_COMM_WORLD);
		}
    } else {
        int parent = (myid - 1) / 2;
        MPI_Recv(&n, 1, MPI_INT, parent, 0, MPI_COMM_WORLD, &status);
		//改到這裡
        if ((myid * 2 + 1) < numprocs) {
            if ((myid * 2 + 1) < numprocs) {
                MPI_Send(&n, 1, MPI_INT, myid * 2 + 1, 0, MPI_COMM_WORLD);
            }
            if ((myid * 2 + 2) < numprocs) {
                MPI_Send(&n, 1, MPI_INT, myid * 2 + 2, 0, MPI_COMM_WORLD);
            }
        }
    }
	MPI_Get_processor_name(processor_name,&namelen);
	fprintf(stderr,"Process %d on %s, n=%d\n", myid, processor_name, n);
    MPI_Finalize();
    return 0;
}

