#include "autobahn.h"

/**
 * @brief Performs squaring of a word.
 * 
 * @param result [out] Pointer to the resulting Bigint.
 * @param operand_x [in] Operand word to be squared.
 */
static void word_squaring(Bigint** result, Word operand_x) 
{
    Word bitlen_half = BITLEN_OF_WORD / 2;
    Bigint* tmp_result = NULL;
    
    /* Allocate memory for Bigint */
    bigint_new(&tmp_result, 2);

    /* Initialize variables */
    Word x_high = operand_x >> bitlen_half;               // A1: Upper half of operand_x
    Word x_low = (x_high << bitlen_half) ^ operand_x;     // A0: Lower half of operand_x
    Word carry = 0;

    /* Compute high and low */
    tmp_result->digits[1] = x_high * x_high; // A1A1
    tmp_result->digits[0] = x_low * x_low;   // A0A0

    /* Divide the middle */
    Word x_middle_low = (x_low * x_high) << bitlen_half;  // Upper half of A1A0
    Word x_middle_high = (x_low * x_high) >> bitlen_half; // Lower half of A1A0

    /* Compute the middle */
    carry = GET_MSB(x_middle_low);    // Carry of one-bit left shift
    x_middle_low = x_middle_low << 1; // Left shift
    x_middle_high = (x_middle_high << 1) + carry; // Left shift with carry, we have 2A1A0

    /* Compute the result */
    tmp_result->digits[0] += x_middle_low;          // A0A0 + Lower A1A0
    carry = tmp_result->digits[0] < x_middle_low;   // Set carry
    tmp_result->digits[1] += x_middle_high + carry; // A1A1 + Upper A1A0 + carry

    /* Finalize the result */
    bigint_refine(tmp_result);
    bigint_copy(result, tmp_result);
    
    /* Free allocated memory for Bigint */
    bigint_delete(&tmp_result); 
}


/**
 * @brief Performs multiplication of two words.
 * 
 * @param result [out] Pointer to the resulting Bigint.
 * @param operand_x [in] First operand word.
 * @param operand_y [in] Second operand word.
 */
static void word_multiplication(Bigint** result, const Word operand_x, const Word operand_y) 
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
 * @brief Performs textbook squaring of a Bigint.
 * 
 * @param result [out] Pointer to the resulting Bigint.
 * @param operand_x [in] Pointer to the operand Bigint to be squared.
 */
void bigint_squaring_textbook(Bigint** result, const Bigint* operand_x)
{
    Bigint* tmp_result = NULL; 
    Bigint* diagonal = NULL; 
    Bigint* diagonal_sum = NULL; 
    Bigint* upper = NULL; 
    Bigint* upper_sum = NULL;

    /* Allocate memory for Bigint */
    bigint_new(&tmp_result, operand_x->digit_num * 2); 
    bigint_new(&diagonal, 2);  
    bigint_new(&diagonal_sum, 1); 
    bigint_new(&upper, 2); 
    bigint_new(&upper_sum, 1);
 
    /* Initialization */
    bigint_set_zero(&diagonal_sum); 
    bigint_set_zero(&upper_sum);
    
    /* Compute squaring */
    for (int idx = 0; idx < operand_x->digit_num; idx++) 
    {
        word_squaring(&diagonal, operand_x->digits[idx]);       // Ai * Ai
        bigint_expand(&diagonal, diagonal, idx + idx);          // Set correct index
        bigint_addition(&diagonal_sum, diagonal_sum, diagonal); // sum(Ai * Ai)
        
        for (int jdx = idx + 1; jdx < operand_x->digit_num; jdx++) 
        {
            word_multiplication(&upper, operand_x->digits[idx], operand_x->digits[jdx]); // Ai * Aj
            bigint_expand(&upper, upper, jdx + idx);       // Set correct index
            bigint_addition(&upper_sum, upper_sum, upper); // sum(Ai * Aj)
        }
    }

    /* Final computation */
    bigint_expand_one_bit(&upper_sum, upper_sum);          // 2 * sum(Ai * Aj)
    bigint_addition(&tmp_result, diagonal_sum, upper_sum); // sum(Ai * Ai) + 2 * sum(Ai * Aj)

    /* Get the result */
    bigint_refine(tmp_result);
    bigint_copy(result, tmp_result);

    /* Free allocated memory for Bigint */
    bigint_delete(&tmp_result); 
    bigint_delete(&diagonal_sum); 
    bigint_delete(&upper_sum);
    bigint_delete(&diagonal); 
    bigint_delete(&upper); 
}


/**
 * @brief Performs squaring of a Bigint using the Karatsuba algorithm.
 * 
 * @param result [out] Pointer to the resulting Bigint.
 * @param operand_x [in] Pointer to the operand Bigint to be squared.
 */
void bigint_squaring_karatsuba(Bigint** result, Bigint* operand_x)
{
    /* Special case: multiplication by zero */
    if (bigint_is_zero(operand_x)) {
        bigint_set_zero(result);
        return;
    }

    /* Recursion stop condition */
    if (operand_x->digit_num <= 4) {
        bigint_squaring_textbook(result, operand_x);
        return;
    }

    /* Declare temporary Bigints for recursive computations */
    Bigint* x_high = NULL; 
    Bigint* x_low = NULL;
    Bigint* result_high = NULL; 
    Bigint* result_middle = NULL;
    Bigint* result_low = NULL;
    Bigint* tmp_result = NULL; 

    /* Allocate memory for temporary Bigints */
    bigint_new(&x_high, 1); 
    bigint_new(&x_low, 1);
    bigint_new(&result_high, 1); 
    bigint_new(&result_low, 1);
    bigint_new(&result_middle, 1);
    bigint_new(&tmp_result, 1);

    /* Determine divide size */
    Word digit_num_half = (operand_x->digit_num + 1) >> 1;
    
    /* Divide operands into upper and lower parts */
    bigint_compress(&x_high, operand_x, digit_num_half);    // A0
    bigint_copy_part(&x_low, operand_x, 0, digit_num_half); // A1

    /* Compute high value, low value, and middle value */
    bigint_squaring_karatsuba(&result_high, x_high);                // A0A0
    bigint_squaring_karatsuba(&result_low, x_low);                  // A1A1
    bigint_multiplication_karatsuba(&result_middle, x_high, x_low); // A1A0
    bigint_expand_one_bit(&result_middle, result_middle);           // 2A1A0

    /* Set index; middle value is shifted one bit more since the middle value is doubled */
    bigint_expand(&result_high, result_high, 2 * digit_num_half);     // A0A0w^(2n)
    bigint_expand(&result_middle, result_middle, digit_num_half);     // 2A1A0w^(n)

    /* Compute the final result */
    bigint_addition(&tmp_result, result_high, result_low);   // A0A0w^(2n) + A1A1
    bigint_addition(&tmp_result, tmp_result, result_middle); // A0A0w^(2n) + 2A1A0w^n + A1A1

    /* Get the result */
    bigint_refine(tmp_result);
    bigint_copy(result, tmp_result);

    /* Free allocated memory for Bigint */
    bigint_delete(&x_high); 
    bigint_delete(&x_low);
    bigint_delete(&result_high); 
    bigint_delete(&result_middle);
    bigint_delete(&result_low);
    bigint_delete(&tmp_result);
}
