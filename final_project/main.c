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
	MPI_Barrier(MPI_COMM_WORLD);	
	mpz_t N, r;
	mpz_inits(N, r, NULL);

	
	karatsuba(&N, rank, p, q);
	MPI_Barrier(MPI_COMM_WORLD);

	mpz_sub(r, N, p);
	mpz_sub(r, r, q);
	mpz_add_ui(r, r, 1);
	MPI_Barrier(MPI_COMM_WORLD);

	mpz_t* e = gen_e(r, rank, size);
	MPI_Barrier(MPI_COMM_WORLD);
	mpz_t* d = gen_d(*e, r);
	if(rank == 0){
		gmp_printf("public key:(%Zd, %Zd)\n", N, e);
		gmp_printf("private key: (%Zd, %Zd)\n", N, d);
	}
	MPI_Finalize();
    return 0;
}

