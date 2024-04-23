#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
    int n, myid, numprocs;
    int namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);

    if (myid == 0)
        n = 12345;

    if (myid == 0) {
        MPI_Send(&n, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Send(&n, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
    }

    while (myid < numprocs - 1) {
        int dest1 = myid * 2 + 1;
        int dest2 = myid * 2 + 2;

        if (dest1 < numprocs) {
            MPI_Recv(&n, 1, MPI_INT, myid / 2, 0, MPI_COMM_WORLD, &status);
            MPI_Send(&n, 1, MPI_INT, dest1, 0, MPI_COMM_WORLD);
        }

        if (dest2 < numprocs) {
            MPI_Recv(&n, 1, MPI_INT, myid / 2, 0, MPI_COMM_WORLD, &status);
            MPI_Send(&n, 1, MPI_INT, dest2, 0, MPI_COMM_WORLD);
        }

        // 让每个进程只处理一次消息并退出循环
        break;
    }

    MPI_Get_processor_name(processor_name, &namelen);
    fprintf(stderr, "Process %d on %s, n=%d\n", myid, processor_name, n);
    MPI_Finalize();

    return 0;
}

