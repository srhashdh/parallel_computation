#include <stdio.h>
#include <mpi.h>
#include <gmp.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

void karatsuba(mpz_t *N, int rank, mpz_t p, mpz_t q){
	size_t size = fmax(mpz_sizeinbase(p, 10), mpz_sizeinbase(q, 10));
    size_t halfSize = size / 2;
	mpz_t high_p, low_p, high_q, low_q, temp1, temp2, z0, z1, z2;
    mpz_inits(high_p, low_p, high_q, low_q, temp1, temp2, z0, z1, z2, NULL);

	mpz_tdiv_q_2exp(high_p, p, halfSize);
    mpz_tdiv_r_2exp(low_p, p, halfSize);
    mpz_tdiv_q_2exp(high_q, q, halfSize);
    mpz_tdiv_r_2exp(low_q, q, halfSize);
	MPI_Status status;
	switch(rank){
		case(1):
			mpz_mul(z2, high_p, high_q);
			
			char *z2_str = mpz_get_str(NULL, 10, z2);
            int z2_length = strlen(z2_str) + 1;
            MPI_Send(&z2_length, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(z2_str, z2_length, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
			printf("rank %d sended\n", rank);
            free(z2_str);
			break;
		case(2):
			mpz_mul(z0, low_p, low_q);


			char *z0_str = mpz_get_str(NULL, 10, z0);
            int z0_length = strlen(z0_str) + 1;
            MPI_Send(&z0_length, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
            MPI_Send(z0_str, z0_length, MPI_CHAR, 0, 3, MPI_COMM_WORLD);
			printf("rank %d sended\n", rank);
            free(z0_str);

			break;
		case(3):
			mpz_add(temp1, high_p, low_p);
			mpz_add(temp2, high_q, low_q);
			mpz_mul(z1, temp1, temp2);

			char *z1_str = mpz_get_str(NULL, 10, z1);
            int z1_length = strlen(z1_str) + 1;
            MPI_Send(&z1_length, 1, MPI_INT, 0, 4, MPI_COMM_WORLD);
            MPI_Send(z1_str, z1_length, MPI_CHAR, 0, 5, MPI_COMM_WORLD);
			printf("rank %d sended\n", rank);
            free(z1_str);

			break;
		case(0):
			int str_length;
            MPI_Recv(&str_length, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
			char *z2_str_0 = malloc(z2_length);
            MPI_Recv(z2_str_0, str_length, MPI_CHAR, 1, 1, MPI_COMM_WORLD, &status);
            mpz_set_str(z2, z2_str_0, 10);
            free(z2_str_0);

			printf("z2 send success\n");
            MPI_Recv(&str_length, 1, MPI_INT, 2, 2, MPI_COMM_WORLD, &status);
            char *z1_str_0 = malloc(z1_length);
            MPI_Recv(z1_str_0, str_length, MPI_CHAR, 2, 3, MPI_COMM_WORLD, &status);
            mpz_set_str(z1, z1_str_0, 10);
            free(z1_str_0);
			printf("z1 send success\n");

            MPI_Recv(&str_length, 1, MPI_INT, 3, 4, MPI_COMM_WORLD, &status);
            char *z0_str_0 = malloc(z0_length);
            MPI_Recv(z0_str_0, str_length, MPI_CHAR, 3, 5, MPI_COMM_WORLD, &status);
            mpz_set_str(z0, z0_str_0, 10);
            free(z0_str_0);
			printf("z1 send success\n");
			mpz_sub(z1, z1, z2);
			mpz_sub(z1, z1, z0);
			mpz_mul_2exp(temp1, z2, size);
			mpz_mul_2exp(temp2, z1, halfSize);
			mpz_add(*N, temp1, temp2);
			mpz_add(*N, *N, z0);
			break;
		default:
			break;
	}
	mpz_clears(high_p, low_p, high_q, low_q, temp1, temp2, z0, z1, z2, NULL);
}
