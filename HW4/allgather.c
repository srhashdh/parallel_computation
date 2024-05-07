#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <mpi.h>
int My_Allgather1(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                 void *recvbuf, int recvcount, MPI_Datatype recvtype,
                 MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    memcpy((char*)recvbuf + rank * recvcount, sendbuf, sendcount);

    for (int i = 0; i < size - 1; i++) {
        MPI_Sendrecv((char*)recvbuf + ((rank - i + size) % size) * recvcount, recvcount, recvtype, (rank + 1) % size, 0,
                     (char*)recvbuf + ((rank - i - 1 + size) % size) * recvcount, recvcount, recvtype,
                     (rank - 1 + size) % size, 0, comm, MPI_STATUS_IGNORE);
    }

    return MPI_SUCCESS;
}
int My_Allgather2(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                 void *recvbuf, int recvcount, MPI_Datatype recvtype,
                 MPI_Comm comm) {
    int rank, size;
    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &size);

    MPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, 0, comm);

    MPI_Bcast(recvbuf, recvcount * size, recvtype, 0, comm);

    return MPI_SUCCESS;
}




typedef unsigned char byte;

static void check(int nprocs, int myrank, size_t size, byte *buffer) {
    size_t i, j;
    for (j = 0; j < nprocs; j++) {
        for (i = 0; i < size; i++) {
            if (buffer[j * size + i] != ((j + 1) & 255)) {
                fprintf(stderr, "Process %d: incorrect value at block %d, position %d\n", myrank, j, i);
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
    }
}
int main(int argc, char **argv){
	int nprocs, myrank;
	byte *send_buffer, *recv_buffer;
	size_t size = 0 /* this makes gcc happy */ ;
	double time0, time1;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	if (argc != 2) {
    	if (myrank == 0)
			fprintf(stderr, "Usage:   %s buffersize[K|M|G]\n", argv[0]);
    		/*MPI_Abort(MPI_COMM_WORLD, 1);*/
    	MPI_Finalize();
    	exit(1);
	}
	else {
		char *p;
		size = strtol(argv[1], &p, 10);
		switch (toupper(*p)) {
    		case 'G':
        		size *= 1024;
			case 'M':
    			size *= 1024;
			case 'K':
    			size *= 1024;
				break;
		}
	}
	if (size <= 0) {
		fprintf(stderr, "Process %d: invalid size %d\n", myrank, size);
		MPI_Abort(MPI_COMM_WORLD, 1);
	}
	
	if (myrank == 0) {
    	fprintf(stderr, "Allgather with %d processes, buffer size: %d\n", nprocs, size);
	}

	send_buffer = malloc(size);
	recv_buffer = malloc(nprocs * size);
	if (send_buffer == NULL || recv_buffer == NULL) {
		fprintf(stderr, "Process %d: memory allocation error!\n", myrank);
    	MPI_Abort(MPI_COMM_WORLD, 1);
	}
	memset(send_buffer, myrank + 1, size);


	memset(recv_buffer, 0, nprocs * size);
	MPI_Barrier(MPI_COMM_WORLD);
	time0 = MPI_Wtime();
	My_Allgather1(send_buffer, size, MPI_BYTE, recv_buffer, size, MPI_BYTE, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	time1 = MPI_Wtime();
	if (myrank == 0)
    	fprintf(stderr, "The circular algorithm: wall time = %lf\n", time1 - time0);
	check(nprocs, myrank, size, recv_buffer);



	memset(recv_buffer, 0, nprocs * size);
    MPI_Barrier(MPI_COMM_WORLD);
    time0 = MPI_Wtime();
    My_Allgather2(send_buffer, size, MPI_BYTE, recv_buffer, size, MPI_BYTE, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    time1 = MPI_Wtime();
    if (myrank == 0)
        fprintf(stderr, "The tree algorithm: wall time = %lf\n", time1 - time0);
    check(nprocs, myrank, size, recv_buffer);

	memset(recv_buffer, 0, nprocs * size); MPI_Barrier(MPI_COMM_WORLD);
	time0 = MPI_Wtime();
	MPI_Allgather(send_buffer, size, MPI_BYTE, recv_buffer, size, MPI_BYTE, MPI_COMM_WORLD);
	
	MPI_Barrier(MPI_COMM_WORLD);
	time1 = MPI_Wtime();
	if (myrank == 0)
    	fprintf(stderr, "MPI_Allgather: wall time = %lf\n", time1 - time0);
	check(nprocs, myrank, size, recv_buffer);

	MPI_Finalize();
	return 0;


}
