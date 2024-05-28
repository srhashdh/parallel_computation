#include <stdio.h>
#include <gmp.h>
#include "define.h"
#include <mpi.h>
int main(int argc, char *argv[]) {
    mpz_t p, q;
	mpz_init(p);
    mpz_init(q);
	int rank, size;
	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
	gen_prime(&p, &q, rank, size);
	if(rank == 0){
		gmp_printf("%Zd\n", p);
		gmp_printf("%Zd\n", q);
		printf("\n");
	}
	MPI_Barrier(MPI_COMM_WORLD);	
	mpz_t N, phi_N;
	mpz_inits(N, phi_N, NULL);
	karatsuba(&N, rank, p, q);
	if(rank == 0){
		gmp_printf("%Zd\n", N);
	}
	MPI_Finalize();
    return 0;
}

