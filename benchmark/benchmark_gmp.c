#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include </opt/homebrew/Cellar/gmp/6.2.1_1/include/gmp.h> //! compile: gcc *.c -L/opt/homebrew/Cellar/gmp/6.2.1_1/lib -lgmp

void gmp_benchmark(uint32_t bit_length)
{
    mpz_t operand_x;
    mpz_t operand_y;
    mpz_t operand_y_half;
    mpz_t operand_z;
    mpz_t result;
    gmp_randstate_t state;
    time_t start;
    time_t end;
    double time_result;
    size_t count = 100;

    /* Allocate memory */
    mpz_init(operand_x);
    mpz_init(operand_y);
    mpz_init(operand_y_half);
    mpz_init(operand_z); 
    mpz_init(result);
    
    /* Init random function */
    gmp_randinit_default(state);

    /* Generate random number */
    mpz_urandomb(operand_x, state, bit_length);
    mpz_urandomb(operand_y, state, bit_length);
    mpz_urandomb(operand_y_half, state, bit_length / 2);
    mpz_urandomb(operand_z, state, bit_length);

    /* time check: multiplication textbook */
    start = clock();
    for(size_t i = 0; i < count; i++) mpz_mul(result, operand_x, operand_y);
    end = clock();
    time_result = (double)(end - start) / CLOCKS_PER_SEC;
    printf("time multiplication : %f\n", time_result);

    /* time check: squaring textbook */
    start = clock();
    for(size_t i = 0; i < count; i++) mpz_pow_ui(result, operand_x, 2);
    end = clock();
    time_result = (double)(end - start) / CLOCKS_PER_SEC;
    printf("time squaring       : %f\n", time_result);

    /* time check: division word */
    start = clock();
    for(size_t i = 0; i < count; i++) mpz_div(result, operand_x, operand_y_half);
    end = clock();
    time_result = (double)(end - start) / CLOCKS_PER_SEC;
    printf("time division       : %f\n", time_result);

    /* time check: barrett */
    start = clock();
    for(size_t i = 0; i < count; i++) mpz_mod(result, operand_x, operand_y_half);
    end = clock();
    time_result = (double)(end - start) / CLOCKS_PER_SEC;
    printf("time reduction      : %f\n", time_result);

    /* time check: exponetiation left to right */
    start = clock();
    mpz_powm(result, operand_x, operand_y, operand_z);
    end = clock();
    time_result = (double)(end - start) / CLOCKS_PER_SEC;
    printf("time exponetiation  : %f\n", time_result);

    /* Free memory */
    mpz_clear(operand_x);
    mpz_clear(operand_y);
    mpz_clear(operand_y_half);
    mpz_clear(operand_z);
    mpz_clear(result);
    gmp_randclear(state);
}

int main()
{
    gmp_benchmark(2048);
}