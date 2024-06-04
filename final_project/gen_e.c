#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include <time.h>
#include <mpi.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
void serialize_mpz(mpz_t e, unsigned char** send_buffer, int* send_size) {
	*send_size = (mpz_sizeinbase(e, 2) + 7) / 8;  // Size in bytes
    *send_buffer = (unsigned char*)malloc(*send_size);
    mpz_export(*send_buffer, NULL, 1, 1, 1, 0, e);  // Export to bytes
}

void deserialize_mpz(mpz_t recv_data, unsigned char* buffer, int size) {
    mpz_import(recv_data, size, 1, 1, 1, 0, buffer);  // Import from bytes
}

void generate_random(mpz_t e, gmp_randstate_t state, mpz_t r) {
    mpz_urandomm(e, state, r);
}

mpz_t* gen_e(mpz_t r, int rank, int size) {
    mpz_t gcd;
    mpz_init(gcd);
	//unsigned char* e_buffer;
    //int e_size;
	bool found = false;
	char *e_str = NULL;
    int e_length = 0;
	mpz_t* e = (mpz_t*)malloc(sizeof(mpz_t));
	mpz_init(*e);
    while (!found) {
		mpz_t *candidate_es;
		if(rank == 0){
			int array_size = size;
   			candidate_es = malloc(array_size * sizeof(mpz_t));

    		for (int i = 0; i < array_size; i++) {
        		mpz_init(candidate_es[i]);
    		}		
		}

		mpz_t candidate_e;
		mpz_init(candidate_e);
    	gmp_randstate_t state;
		gmp_randinit_default(state);
		gmp_randseed_ui(state, time(NULL) + rank);
		generate_random(candidate_e, state, r);
		gmp_randclear(state);	
		unsigned char* send_buffer;
    	int send_size;
    	serialize_mpz(candidate_e, &send_buffer, &send_size);

    	int* recv_sizes = NULL;
    	if (rank == 0) {
        	recv_sizes = (int*)malloc(size * sizeof(int));
    	}
    	MPI_Gather(&send_size, 1, MPI_INT, recv_sizes, 1, MPI_INT, 0, MPI_COMM_WORLD);

		unsigned char* recv_buffer = NULL;
		int* displs = NULL;
		if (rank == 0) {
    		int total_size = 0;
    		displs = (int*)malloc(size * sizeof(int));
    		for (int i = 0; i < size; i++) {
        		displs[i] = total_size;
        		total_size += recv_sizes[i];
    		}
    		recv_buffer = (unsigned char*)malloc(total_size);
		}	

		MPI_Gatherv(send_buffer, send_size, MPI_UNSIGNED_CHAR, recv_buffer, recv_sizes, displs, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
		if (rank == 0) {
        	mpz_t recv_data;
        	mpz_init(recv_data);
        	for (int i = 0; i < size; i++) {
            	deserialize_mpz(recv_data, recv_buffer + displs[i], recv_sizes[i]);
				mpz_set(candidate_es[i], recv_data);
				mpz_init(recv_data);
			}
			for(int i = 0; i < size; i++){
				mpz_gcd(gcd, candidate_es[i], r);
				if(mpz_cmp_ui(gcd, 1) == 0){
					mpz_set(*e, candidate_es[i]);
					found = true;
					e_str = mpz_get_str(NULL, 10, *e);
            		e_length = strlen(e_str) + 1;
					break;
				}
			}
        }
		MPI_Bcast(&found, 1, MPI_C_BOOL, 0, MPI_COMM_WORLD);
		MPI_Bcast(&e_length, 1, MPI_INT, 0, MPI_COMM_WORLD);
		if (rank != 0) {
        	e_str = (char *)malloc(e_length);
    	}
		MPI_Bcast(e_str, e_length, MPI_CHAR, 0, MPI_COMM_WORLD);
		if(rank != 0){
			mpz_set_str(*e, e_str, 10);
		}
    }
	return e;
}

