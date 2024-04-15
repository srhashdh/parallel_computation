#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 1024*1024

int main(int argc, char *argv[]) {
    int myrank, nprocs, src, dest, tag;
    double *A, *B, sum = 0.0;
    MPI_Status status;
    MPI_Request req_send, req_recv;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    A = (double*)malloc(N * sizeof(double));
    B = (double*)malloc(N * sizeof(double));

    for (int i = 0; i < N; i++) {
        A[i] = (double)myrank;
    }

    src = myrank - 1;
    if (src < 0) src = nprocs - 1;
    dest = myrank + 1;
    if (dest >= nprocs) dest = 0;

    tag = 111;

    if (myrank % 2 == 0) {
        MPI_Isend(A, N, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD, &req_send);
        MPI_Irecv(B, N, MPI_DOUBLE, src, tag, MPI_COMM_WORLD, &req_recv);
        MPI_Wait(&req_recv, &status);
    }
    else {
        MPI_Irecv(B, N, MPI_DOUBLE, src, tag, MPI_COMM_WORLD, &req_recv);
        MPI_Isend(A, N, MPI_DOUBLE, dest, tag, MPI_COMM_WORLD, &req_send);
        MPI_Wait(&req_recv, &status);
    }

    for (int i = 0; i < N; i++) {
        sum += B[i];
    }

    printf("Process %d:  value = %lf\n", myrank, sum / N);

    free(A);
    free(B);

    MPI_Finalize();
    return 0;
}

