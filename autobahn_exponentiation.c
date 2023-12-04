#include "autobahn.h"

/**
 * @brief Performs modular exponentiation using the left-to-right method.
 * 
 * @param result [out] Result of the modular exponentiation.
 * @param base [in] Base value.
 * @param exponent [in] Exponent value.
 * @param modular [in] Modulus value.
 */
void bigint_exponentiation_modular_left_to_right(Bigint** result, const Bigint* base, const Bigint* exponent, const Bigint* modular)
{
    /* Ensure that base and exponent are non-negative */
    if (base->sign == NEGATIVE || exponent->sign == NEGATIVE) {
        printf("Invalid Case: Base or exponent must be positive.\n");
        return;
    }

    Bigint* result_tmp = NULL;
    Bigint* barrett_pre_compute = NULL;

    /* Allocate Bigint */
    bigint_new(&result_tmp, 1);
    bigint_new(&barrett_pre_compute, 1);

    /* Initialization */
    bigint_set_one(&result_tmp);
    bigint_reduction_barrett_pre_computed(&barrett_pre_compute, modular);

    /* Iteration count */
    Word digit_idx = exponent->digit_num;

    /* Left-to-right: conditional multiplication */
    while (digit_idx--)
    {
        Word bit_idx = BITLEN_OF_WORD; // iteration count

        while (bit_idx--)
        {
            bigint_squaring_textbook(&result_tmp, result_tmp); // always squaring.
            bigint_reduction_barrett(&result_tmp, result_tmp, modular, barrett_pre_compute); // modular     

            if (GET_BIT(exponent->digits[digit_idx], bit_idx) == 1) {
                bigint_multiplication_textbook(&result_tmp, result_tmp, base); // conditional multiplication.
                bigint_reduction_barrett(&result_tmp, result_tmp, modular, barrett_pre_compute); // modular 
            }    
        }
    }

    /* Get result */
    bigint_refine(result_tmp);
    bigint_copy(result, result_tmp);

    /* Free Bigint */
    bigint_delete(&result_tmp);
    bigint_delete(&barrett_pre_compute);
}

/**
 * @brief Performs modular exponentiation using the Montgomery Ladder method.
 * 
 * @param result [out] Result of the modular exponentiation.
 * @param base [in] Base value.
 * @param exponent [in] Exponent value.
 * @param modular [in] Modulus value.
 */
void bigint_exponentiation_modular_montgomery_ladder(Bigint** result, const Bigint* base, const Bigint* exponent, const Bigint* modular)
{
    /* Ensure that base and exponent are non-negative */
    if (base->sign == NEGATIVE || exponent->sign == NEGATIVE) {
        printf("Invalid Case: Base or exponent must be positive.\n");
        return;
    }

    /* Allocate Bigint and initialize for the algorithm */
    Bigint* left = NULL; // This will be the result.
    Bigint* right = NULL;
    Bigint* barrett_pre_compute = NULL;

    /* Allocate Bigint */
    bigint_new(&left, 1);
    bigint_new(&right, 1);
    bigint_new(&barrett_pre_compute, 1);

    /* Initialization */
    bigint_set_one(&left);     // L = 1
    bigint_copy(&right, base); // R = x
    bigint_reduction_barrett_pre_computed(&barrett_pre_compute, modular);

    /* Iteration count */
    Word digit_idx = exponent->digit_num;

    /* Left-to-right: conditional multiplication */
    while (digit_idx--)
    {
        Word bit_idx = BITLEN_OF_WORD; // iteration count

        while (bit_idx--)
        {
            if (GET_BIT(exponent->digits[digit_idx], bit_idx) == 1) {
                bigint_multiplication_textbook(&left, left, right);                    // L <- LR
                bigint_reduction_barrett(&left, left, modular, barrett_pre_compute);   // L <- LR mod n
                bigint_squaring_textbook(&right, right);                               // R <- RR
                bigint_reduction_barrett(&right, right, modular, barrett_pre_compute); // R <- RR mod n
            } 
            else {
                bigint_multiplication_textbook(&right, right, left);                   // R <- LR
                bigint_reduction_barrett(&right, right, modular, barrett_pre_compute); // R <- LR mod n    
                bigint_squaring_textbook(&left, left);                                 // L <- LL
                bigint_reduction_barrett(&left, left, modular, barrett_pre_compute);   // L <- LL mod n
            }
        }
    }

    /* Get result */
    bigint_refine(left);
    bigint_copy(result, left);

    /* Free Bigint */
    bigint_delete(&left);
    bigint_delete(&right);
    bigint_delete(&barrett_pre_compute);
}