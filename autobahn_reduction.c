#include "autobahn.h"

/**
 * @brief Computes the pre-computed value for Barrett reduction.
 *
 * @param barrett_pre_computed [out] Pointer to store the pre-computed value.
 * @param modular [in] The modulus for Barrett reduction.
 */
void bigint_reduction_barrett_pre_computed(Bigint** barrett_pre_computed, const Bigint* modular) 
{
    /* Allocate memory for intermediate results */
    Bigint* remainder = NULL; // Remainder during computation
    Bigint* quotient = NULL;  // Pre-computed value for Barrett reduction
    Bigint* dividend = NULL;  // = W^(2n)
    bigint_new(&remainder, 1);
    bigint_new(&quotient, 1);
    bigint_new(&dividend, 1);

    /* Initialize the dividend */
    bigint_set_one(&dividend);
    bigint_expand(&dividend, dividend, 2 * modular->digit_num); // W^(2n)

    /* Compute division: quotient */
    bigint_division_word_long(&quotient, &remainder, dividend, modular); // W^(2n) / N

    /* Get the final result */
    bigint_refine(quotient);
    bigint_copy(barrett_pre_computed, quotient);

    /* Free allocated memory */
    bigint_delete(&remainder);
    bigint_delete(&quotient);
    bigint_delete(&dividend);
}

/**
 * @brief Performs Barrett reduction on a given dividend using a pre-computed value.
 *
 * @param result [out] Pointer to store the result of Barrett reduction.
 * @param dividend [in] The dividend for reduction.
 * @param modular [in] The modulus for Barrett reduction.
 * @param pre_computed [in] The pre-computed value.
 */
void bigint_reduction_barrett(Bigint** result, const Bigint* dividend, const Bigint* modular, const Bigint* pre_computed) 
{ 
    /* Check for invalid parameter: A is in the range [0, 2^(W * 2n) - 1] */
    if (dividend->digit_num > modular->digit_num * 2) {
        printf("Barrett reduction not applicable: A exceeds 2^(W * 2n)\n");
        return;
    }

    /* Allocate memory for intermediate results */
    Bigint* quotient = NULL;  // Quotient during Barrett reduction
    Bigint* remainder = NULL; // Result of Barrett reduction
    bigint_new(&quotient, 1);
    bigint_new(&remainder, 1);

    /* Barrett reduction: Compute quotient (Q) */
    bigint_compress(&quotient, dividend, modular->digit_num - 1);      // Q <- A >> W^(n-1)
    bigint_multiplication_karatsuba(&quotient, quotient, pre_computed); // Q <- (A >> W^(n-1)) * T
    bigint_compress(&quotient, quotient, modular->digit_num + 1);      // Q <- ((A >> W^(n-1)) * T) >> W^(n+1)

    /* Barrett reduction: Compute remainder (R) */
    bigint_multiplication_karatsuba(&remainder, quotient, modular); // R <- Q * N
    bigint_subtraction(&remainder, dividend, remainder);           // R <- A - Q * N

    /* Barrett reduction: Find the correct result for remainder */
    while (bigint_compare(remainder, modular) != LEFT_IS_SMALL)
        bigint_subtraction(&remainder, remainder, modular); // Make R if remainder is R + N or R + 2N

    /* Get the final result */
    bigint_refine(remainder);
    bigint_copy(result, remainder);

    /* Free allocated memory */
    bigint_delete(&quotient);
    bigint_delete(&remainder);
}
