#include <stdio.h>
#include <mpi.h>
#include <gmp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#define BIT_SIZE 1024

void generate_large_prime(mpz_t prime, int rank) {
    gmp_randstate_t state;
    unsigned long int bit_size = BIT_SIZE;

    mpz_init(prime);
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL) + rank);
    mpz_urandomb(prime, state, bit_size);
    mpz_nextprime(prime, prime);
    gmp_randclear(state);
}

void gen_prime(mpz_t *p, mpz_t *q, int rank, int size) {
    MPI_Status status;

    bool found = false;
	char *p_str = NULL;
    int p_length = 0;
	
	char *q_str = NULL;
    int q_length = 0;
    if (rank == 1 || rank == 2) {
        while (!found) {
            mpz_t prime;
            mpz_init(prime);
            generate_large_prime(prime, rank);


            char *prime_str = mpz_get_str(NULL, 10, prime);
            int str_length = strlen(prime_str) + 1;
            MPI_Send(&str_length, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(prime_str, str_length, MPI_CHAR, 0, 1, MPI_COMM_WORLD);

            free(prime_str);
            mpz_clear(prime);

            MPI_Recv(&found, 1, MPI_C_BOOL, 0, 2, MPI_COMM_WORLD, &status);
        }
    } else if (rank == 0) {
        while (!found) {
            mpz_init(*p);
            mpz_init(*q);

            int str_length;
            MPI_Recv(&str_length, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
            char *prime1_str = malloc(str_length);
            MPI_Recv(prime1_str, str_length, MPI_CHAR, 1, 1, MPI_COMM_WORLD, &status);
            mpz_set_str(*p, prime1_str, 10);
            free(prime1_str);


            MPI_Recv(&str_length, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status); 
            char *prime2_str = malloc(str_length);
            MPI_Recv(prime2_str, str_length, MPI_CHAR, 2, 1, MPI_COMM_WORLD, &status);
            mpz_set_str(*q, prime2_str, 10);
            free(prime2_str);
			
            if (mpz_cmp(*p, *q) != 0) {
                found = true;
            }
			p_str = mpz_get_str(NULL, 10, *p);
        	p_length = strlen(p_str) + 1;

			q_str = mpz_get_str(NULL, 10, *q);
            q_length = strlen(q_str) + 1;	

            MPI_Send(&found, 1, MPI_C_BOOL, 1, 2, MPI_COMM_WORLD);
            MPI_Send(&found, 1, MPI_C_BOOL, 2, 2, MPI_COMM_WORLD);
        }

    }
	MPI_Bcast(&p_length, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&q_length, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if (rank != 0) {
        p_str = (char *)malloc(p_length);
		q_str = (char *)malloc(q_length);
    }

    MPI_Bcast(p_str, p_length, MPI_CHAR, 0, MPI_COMM_WORLD);
	mpz_set_str(*p, p_str, 10);
	MPI_Bcast(q_str, q_length, MPI_CHAR, 0, MPI_COMM_WORLD);
    mpz_set_str(*q, q_str, 10);
}

