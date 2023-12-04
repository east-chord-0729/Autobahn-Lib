#include "autobahn.h"

#pragma warning(disable: 28182)
#pragma warning(disable: 6308)

/**
 * @brief Performs multiplication of two words.
 *
 * @param result [out] Pointer to the resulting Bigint.
 * @param operand_x [in] First operand word.
 * @param operand_y [in] Second operand word.
 */
static void word_multiplication(Bigint **result, const Word operand_x, const Word operand_y)
{
    Bigint *tmp_result = NULL;

    /* Allocate Bigint */
    bigint_new(&tmp_result, 2); // this will be the result

    /* Initialize variables */
    Word bitlen_half = BITLEN_OF_WORD / 2;
    Word x_high = operand_x >> bitlen_half;           // Upper half of operand_x
    Word x_low = (x_high << bitlen_half) ^ operand_x; // Lower half of operand_x
    Word y_high = operand_y >> bitlen_half;           // Upper half of operand_y
    Word y_low = (y_high << bitlen_half) ^ operand_y; // Lower half of operand_y

    /* Compute high and low value */
    tmp_result->digits[0] = x_low * y_low;   // A0B0
    tmp_result->digits[1] = x_high * y_high; // A1B1

    /* Compute middle value */
    Word middle = x_high * y_low + y_high * x_low; // A1B0 + A0B1
    Word carry = middle < x_high * y_low;          // set carry.

    /* Divide middle value */
    Word middle_low  = middle << bitlen_half; // Lower half of A1B0 + A0B1 : L
    Word middle_high = (middle >> bitlen_half) + (carry << bitlen_half); // Upper half of A1B0 + A0B1, consider carry : U

    /* Compute the result */
    tmp_result->digits[0] += middle_low;          // A0B0 + L
    carry = tmp_result->digits[0] < middle_low;   // set carry
    tmp_result->digits[1] += middle_high + carry; // A1B1 + U + carry

    /* Get result */
    bigint_refine(tmp_result);
    bigint_copy(result, tmp_result);

    /* Free Bigint */
    bigint_delete(&tmp_result);
}

/**
 * @brief Performs textbook multiplication of two Bigints.
 *
 * @param result [out] Pointer to the resulting Bigint.
 * @param operand_x [in] Pointer to the first operand Bigint.
 * @param operand_y [in] Pointer to the second operand Bigint.
 */
void bigint_multiplication_textbook(Bigint **result, const Bigint *operand_x, const Bigint *operand_y)
{
    /* Special case: multiplication by zero */
    if (bigint_is_zero(operand_x) || bigint_is_zero(operand_y))
    {
        bigint_set_zero(result);
        return;
    }

    Bigint *tmp_result = NULL;
    Bigint *word_mult = NULL;

    /* Allocate memory */
    bigint_new(&tmp_result, operand_x->digit_num + operand_y->digit_num);
    bigint_new(&word_mult, 2);

    /* Multiplication loop */
    for (Word idx_x = 0; idx_x < operand_x->digit_num; idx_x++)
    {
        for (Word idx_y = 0; idx_y < operand_y->digit_num; idx_y++)
        {
            word_multiplication(&word_mult, operand_x->digits[idx_x], operand_y->digits[idx_y]); // Perform word multiplication
            bigint_expand(&word_mult, word_mult, idx_x + idx_y);                                 // Set correct index
            bigint_addition(&tmp_result, tmp_result, word_mult);                                 // Addition to the result
        }
    }

    /* Set sign */
    tmp_result->sign = (operand_x->sign == operand_y->sign) ? POSITIVE : NEGATIVE;

    /* Finalize the result */
    bigint_refine(tmp_result);
    bigint_copy(result, tmp_result);

    /* Free allocated memory */
    bigint_delete(&tmp_result);
    bigint_delete(&word_mult);
}

/**
 * @brief Performs Karatsuba multiplication on two Bigints.
 *
 * @param result [out] Pointer to the resulting Bigint.
 * @param operand_x [in] Pointer to the first operand Bigint.
 * @param operand_y [in] Pointer to the second operand Bigint.
 */
void bigint_multiplication_karatsuba(Bigint **result, const Bigint *operand_x, const Bigint *operand_y)
{
    /* Special case: multiplication by zero */
    if (bigint_is_zero(operand_x) || bigint_is_zero(operand_y))
    {
        bigint_set_zero(result);
        return;
    }

    /* Determine divide size */
    Word digit_num_min = operand_x->digit_num < operand_y->digit_num ? operand_x->digit_num : operand_y->digit_num;
    Word digit_num_max = operand_x->digit_num > operand_y->digit_num ? operand_x->digit_num : operand_y->digit_num;
    Word digit_num_half = (digit_num_max + 1) >> 1;

    /* Recursion stop condition */
    if (digit_num_min <= 4)
    {
        bigint_multiplication_textbook(result, operand_x, operand_y);
        return;
    }

    /* Declare temporary Bigints for recursive computations */
    Bigint *x_low = NULL;
    Bigint *x_high = NULL;
    Bigint *y_low = NULL;
    Bigint *y_high = NULL;
    Bigint *y_lowhigh = NULL;
    Bigint *x_lowhigh = NULL;
    Bigint *result_low = NULL;
    Bigint *result_high = NULL;
    Bigint *result_middle = NULL;
    Bigint *tmp_result = NULL;
    Bigint *tmp_x = NULL;
    Bigint *tmp_y = NULL;

    /* Allocate memory for temporary Bigints */
    bigint_new(&x_low, 1);
    bigint_new(&x_high, 1);
    bigint_new(&y_low, 1);
    bigint_new(&y_high, 1);
    bigint_new(&y_lowhigh, 1);
    bigint_new(&x_lowhigh, 1);
    bigint_new(&result_low, 1);
    bigint_new(&result_high, 1);
    bigint_new(&result_middle, 1);
    bigint_new(&tmp_x, 1);
    bigint_new(&tmp_y, 1);
    bigint_new(&tmp_result, 1);

    /* Copy input operands to temporary Bigints */
    bigint_copy(&tmp_x, operand_x);
    bigint_copy(&tmp_y, operand_y);

    /* Store the original digit number of the input operands */
    Word previous_digit_num_x = tmp_x->digit_num;
    Word previous_digit_num_y = tmp_y->digit_num;

    /* Ensure both operands have the same number of digits for further processing */
    tmp_x->digits = (Word *)realloc(tmp_x->digits, digit_num_half * 2 * SIZE_OF_WORD);
    tmp_y->digits = (Word *)realloc(tmp_y->digits, digit_num_half * 2 * SIZE_OF_WORD);
    tmp_x->digit_num = digit_num_half * 2;
    tmp_y->digit_num = digit_num_half * 2;

    /* Initialize the added digits with zero to guard against trash values */
    for (int i = tmp_x->digit_num; i > previous_digit_num_x; i--)
        tmp_x->digits[i - 1] = 0;
    for (int i = tmp_y->digit_num; i > previous_digit_num_y; i--)
        tmp_y->digits[i - 1] = 0;

    /* Divide operands into upper and lower parts */
    bigint_copy_part(&x_low, tmp_x, 0, digit_num_half);                   // A0
    bigint_copy_part(&y_low, tmp_y, 0, digit_num_half);                   // B0
    bigint_copy_part(&x_high, tmp_x, digit_num_half, digit_num_half * 2); // A1
    bigint_copy_part(&y_high, tmp_y, digit_num_half, digit_num_half * 2); // B1

    /* Compute high value and low value */
    bigint_multiplication_karatsuba(&result_high, x_high, y_high); // A1B1
    bigint_multiplication_karatsuba(&result_low, x_low, y_low);    // A0B0

    /* Compute middle value */
    bigint_subtraction(&x_lowhigh, x_high, x_low);                         // A1 - A0
    bigint_subtraction(&y_lowhigh, y_low, y_high);                         // B1 - B0
    bigint_multiplication_karatsuba(&result_middle, x_lowhigh, y_lowhigh); // (A1 - A0)(B1 - B0)
    bigint_addition(&result_middle, result_middle, result_high);           // (A1 - A0)(B1 - B0) + A1B1
    bigint_addition(&result_middle, result_middle, result_low);            // (A1 - A0)(B1 - B0) + A1B1 + A0B0 = A1B0 + A0B1

    /* Compute final result */
    bigint_expand(&result_high, result_high, digit_num_half * 2); // A1B1 * w^(2n)
    bigint_expand(&result_middle, result_middle, digit_num_half); // (A1B0 + A0B1) * w^n
    bigint_addition(&tmp_result, result_high, result_low);        // A1B1w^(2n) + A0B0
    bigint_addition(&tmp_result, tmp_result, result_middle);      // A1B1w^(2n) + (A1B0 + A0B1)w^n + A0B0

    /* Set the sign of the result */
    tmp_result->sign = (tmp_x->sign == tmp_y->sign) ? POSITIVE : NEGATIVE;

    /* Copy the result to the output parameter */
    bigint_copy(result, tmp_result);

    /* Free allocated memory for temporary Bigints */
    bigint_delete(&x_low);
    bigint_delete(&x_high);
    bigint_delete(&y_low);
    bigint_delete(&y_high);
    bigint_delete(&result_low);
    bigint_delete(&result_high);
    bigint_delete(&result_middle);
    bigint_delete(&x_lowhigh);
    bigint_delete(&y_lowhigh);
    bigint_delete(&tmp_x);
    bigint_delete(&tmp_y);
    bigint_delete(&tmp_result);
}
