#include "autobahn.h"

#pragma warning(disable: 28182)
#pragma warning(disable: 6308)

/**
 * @brief Perform addition of two words with carry.
 * 
 * @param result [out] - The result of the addition.
 * @param next_carry [out] - The carry to be propagated to the next addition.
 * @param current_carry [in] - The carry from the previous addition.
 * @param operand_x [in] - The first operand.
 * @param operand_y [in] - The second operand.
 */
static void word_addition_with_carry(Word* result, Word* next_carry, Word current_carry, Word operand_x, Word operand_y) 
{
    *result     = operand_x + operand_y;        // Calculate sum: r = x + y
    *next_carry = (*result < operand_x);        // Set carry if r < x
    *result     += current_carry;               // Add the carry from the previous addition: r = x + y + c
    *next_carry += (*result < current_carry);   // Set carry if r < c
}

/**
 * @brief Perform unsigned addition of two big integers.
 * 
 * @param result [out] - The resulting big integer after addition.
 * @param operand_x [in] - The first big integer operand.
 * @param operand_y [in] - The second big integer operand.
 */
void bigint_addition_unsigned(Bigint** result, Bigint* operand_x, Bigint* operand_y)
{
    Word current_carry = 0;
    Word next_carry = 0;
    Bigint* tmp_result = NULL;  // This will be the result.
    
    /* Ensure operands have the same number of digits */      
    if (operand_y->digit_num != operand_x->digit_num)
    {
        Word previous_digit_num = operand_y->digit_num; 
        Word new_digit_num = operand_x->digit_num;    

        /* Make same number of digits */
        operand_y->digits = (Word*)realloc(operand_y->digits, (operand_x->digit_num) * SIZE_OF_WORD);
        operand_y->digit_num = new_digit_num;

        /* Guard the trash data */
        while(new_digit_num-- != previous_digit_num) 
                operand_y->digits[new_digit_num] = 0;
    }

    /* Allocate memory for the result */
    bigint_new(&tmp_result, operand_x->digit_num + 1);

    /* Addition word by word with carry */
    for (size_t idx = 0; idx < operand_x->digit_num; idx++) {
        word_addition_with_carry(&(tmp_result->digits[idx]), &next_carry, current_carry, operand_x->digits[idx], operand_y->digits[idx]);
        current_carry = next_carry; // Update the carry.
    }
    
    /* Set the final carry */
    tmp_result->digits[operand_x->digit_num] = current_carry;

    /* Get the final result after refining and copying */
    bigint_refine(operand_y);
    bigint_refine(tmp_result);
    bigint_copy(result, tmp_result);

    /* Free allocated memory */
    bigint_delete(&tmp_result);
}

/**
 * @brief Perform addition of two big integers.
 * 
 * @param result [out] - The resulting big integer after addition.
 * @param operand_x [in] - The first big integer operand.
 * @param operand_y [in] - The second big integer operand.
 */
void bigint_addition(Bigint** result, const Bigint* operand_x, const Bigint* operand_y) 
{
    /* Use temporary variables to avoid modifying the input operands */
    Bigint* tmp_x = NULL;
    Bigint* tmp_y = NULL;

    /* Allocate memory for temporary operands */
    bigint_new(&tmp_x, operand_x->digit_num);
    bigint_new(&tmp_y, operand_y->digit_num);

    /* Copy input operands to temporary variables */
    bigint_copy(&tmp_x, operand_x);
    bigint_copy(&tmp_y, operand_y);

    /* Case: result = operand_x + (-operand_y) */
    if ((tmp_x->sign == POSITIVE && tmp_y->sign == NEGATIVE)) {
        tmp_y->sign = POSITIVE;
        bigint_subtraction(result, tmp_x, tmp_y);
        goto RETURN;
    }
    
    /* Case: result = (-operand_x) + operand_y */
    if (tmp_x->sign == NEGATIVE && tmp_y->sign == POSITIVE) {
        tmp_x->sign = POSITIVE;
        bigint_subtraction(result, tmp_y, tmp_x);
        goto RETURN;
    }

    /* Perform addition using the unsigned addition function */
    if (bigint_compare_abs(tmp_x, tmp_y) == 1)
        bigint_addition_unsigned(result, tmp_x, tmp_y);
    else
        bigint_addition_unsigned(result, tmp_y, tmp_x);
    
    /* Set the sign of the result based on the signs of the operands */
    if (tmp_x->sign == POSITIVE && tmp_y->sign == POSITIVE) (*result)->sign = POSITIVE;
    if (tmp_x->sign == NEGATIVE && tmp_y->sign == NEGATIVE) (*result)->sign = NEGATIVE;

    RETURN:

    /* Free allocated memory */
    bigint_delete(&tmp_x);
    bigint_delete(&tmp_y);
}