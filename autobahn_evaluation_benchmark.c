#include "autobahn.h"
#include "time.h"

void bigint_benchmark(Word bit_length)
{
    /* init operands and result of operations */
    Bigint *operand_x = NULL;
    Bigint *operand_y = NULL;
    Bigint *operand_y_half = NULL;
    Bigint *operand_z = NULL;
    Bigint *result = NULL;
    Bigint *pre_comp = NULL;
    Bigint *quotient = NULL;
    Bigint *remainder = NULL;

    /* allocate memory */
    bigint_new(&operand_x, 1);
    bigint_new(&operand_y, 1);
    bigint_new(&operand_y_half, 1);
    bigint_new(&operand_z, 1);
    bigint_new(&result, 1);
    bigint_new(&pre_comp, 1);
    bigint_new(&quotient, 1);
    bigint_new(&remainder, 1);
    
    /* Bit length to word length */
    Word new_digit_num = bit_length / BITLEN_OF_WORD;

    /* setting variables */
    bigint_generate_random_number(&operand_x, POSITIVE, new_digit_num);
    bigint_generate_random_number(&operand_y, POSITIVE, new_digit_num);
    bigint_generate_random_number(&operand_y_half, POSITIVE, new_digit_num / 2);
    bigint_generate_random_number(&operand_z, POSITIVE, new_digit_num);
    bigint_reduction_barrett_pre_computed(&pre_comp, operand_y_half);

    /* time variable */
    time_t start;
    time_t end;
    double time_result;
    size_t count = 100;

    /* time check: multiplication textbook */
    start = clock();
    for(size_t i = 0; i < count; i++) bigint_multiplication_textbook(&result, operand_x, operand_y);
    end = clock();
    time_result = (double)(end - start) / CLOCKS_PER_SEC;
    printf("time multiplication textbook    : %f\n", time_result);

    /* time check: multiplication karatsuba */
    start = clock();
    for(size_t i = 0; i < count; i++) bigint_multiplication_karatsuba(&result, operand_x, operand_y);
    end = clock();
    time_result = (double)(end - start) / CLOCKS_PER_SEC;
    printf("time multiplication karatsuba   : %f\n", time_result);

    /* time check: squaring textbook */
    start = clock();
    for(size_t i = 0; i < count; i++) bigint_squaring_textbook(&result, operand_x);
    end = clock();
    time_result = (double)(end - start) / CLOCKS_PER_SEC;
    printf("time squaring textbook          : %f\n", time_result);

    /* time check: squaring karatsuba */
    start = clock();
    for(size_t i = 0; i < count; i++) bigint_squaring_karatsuba(&result, operand_x);
    end = clock();
    time_result = (double)(end - start) / CLOCKS_PER_SEC;
    printf("time squaring karatsuba         : %f\n", time_result);

    /* time check: division binary */
    start = clock();
    for(size_t i = 0; i < count; i++) bigint_division_binary_long(&quotient, &remainder, operand_x, operand_y_half);
    end = clock();
    time_result = (double)(end - start) / CLOCKS_PER_SEC;
    printf("time division binary            : %f\n", time_result);

    /* time check: division word */
    start = clock();
    for(size_t i = 0; i < count; i++) bigint_division_word_long(&quotient, &remainder, operand_x, operand_y_half);
    end = clock();
    time_result = (double)(end - start) / CLOCKS_PER_SEC;
    printf("time division word              : %f\n", time_result);

    /* time check: barrett */
    start = clock();
    for(size_t i = 0; i < count; i++) bigint_reduction_barrett(&remainder, operand_x, operand_y_half, pre_comp);
    end = clock();
    time_result = (double)(end - start) / CLOCKS_PER_SEC;
    printf("time barrett                    : %f\n", time_result);

    /* time check: exponetiation left to right */
    start = clock();
    bigint_exponentiation_modular_left_to_right(&result, operand_x, operand_y, operand_z);
    end = clock();
    time_result = (double)(end - start) / CLOCKS_PER_SEC;
    printf("time exponetiation left to right: %f\n", time_result);

    /* time check: exponetiation montgomery ladder */
    start = clock();
    bigint_exponentiation_modular_montgomery_ladder(&result, operand_x, operand_y, operand_z);
    end = clock();
    time_result = (double)(end - start) / CLOCKS_PER_SEC;
    printf("time exponetiation montgomery   : %f\n", time_result);

    /* free memory */
    bigint_delete(&operand_x);
    bigint_delete(&operand_y);
    bigint_delete(&operand_y_half);
    bigint_delete(&operand_z);
    bigint_delete(&result);
    bigint_delete(&pre_comp);
    bigint_delete(&quotient);
    bigint_delete(&remainder);
}