#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
mpz_t* gen_d(mpz_t e, mpz_t r) {
    mpz_t* d = (mpz_t*)malloc(sizeof(mpz_t));
    mpz_init(*d);
    mpz_invert(*d, e, r);
    return d;
}

