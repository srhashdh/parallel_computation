#ifndef define_H
#define define_H

#include <gmp.h>
int main();
void gen_prime(mpz_t *prime1, mpz_t *prime2, int rank, int size);
void karatsuba(mpz_t *N, int rank, mpz_t p, mpz_t q);

#endif

