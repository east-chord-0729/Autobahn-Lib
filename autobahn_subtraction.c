#include "autobahn.h"

/**
 * @brief Perform word subtraction with borrow.
 * 
 * @param result [out] - The result of the subtraction.
 * @param next_borrow [out] - The borrow to be propagated to the next subtraction.
 * @param current_borrow [in] - The borrow from the previous subtraction.
 * @param operand_x [in] - The minuend.
 * @param operand_y [in] - The subtrahend.
 */
static void word_subtraction_with_borrow(Word* result, Word* next_borrow, Word current_borrow, Word operand_x, Word operand_y) 
{
    *result      = operand_x - operand_y;       // Calculate difference: r = x - y
    *next_borrow = operand_x < operand_y;       // Set borrow if x < y
    *next_borrow += *result < current_borrow;   // Set borrow if r < c
    *result      -= current_borrow;             // Adjust result for the borrow: r = x - y - b
}

/**
 * @brief Perform unsigned subtraction of two big integers.
 * 
 * @param result [out] - The resulting big integer after subtraction.
 * @param operand_x [in] - The minuend.
 * @param operand_y [in] - The subtrahend.
 */
void bigint_subtraction_unsigned(Bigint** result, Bigint* operand_x, Bigint* operand_y)
{
    Word current_borrow = 0;
    Word next_borrow = 0;
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
    bigint_new(&tmp_result, operand_x->digit_num);

    /* Subtraction word by word with borrow */
    for (int idx = 0; idx < operand_x->digit_num; idx++) 
    {
        word_subtraction_with_borrow(&(tmp_result->digits[idx]), &next_borrow, current_borrow, operand_x->digits[idx], operand_y->digits[idx]);
        current_borrow = next_borrow; // Update the borrow.
    }

    /* Get the final result after refining and copying */
    bigint_refine(operand_y);
    bigint_refine(tmp_result);
    bigint_copy(result, tmp_result);

    /* Free allocated memory */
    bigint_delete(&tmp_result);
}

/**
 * @brief Perform subtraction of two big integers.
 * 
 * @param result [out] - The resulting big integer after subtraction.
 * @param operand_x [in] - The first big integer operand.
 * @param operand_y [in] - The second big integer operand.
 */
void bigint_subtraction(Bigint** result, const Bigint* operand_x, const Bigint* operand_y)
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

    /* Case: result = operand_x - (-operand_y) */
    if (tmp_x->sign == POSITIVE && tmp_y->sign == NEGATIVE) {
        tmp_y->sign = POSITIVE;
        bigint_addition(result, tmp_x, tmp_y);
        (*result)->sign = POSITIVE;
        goto RETURN;
    }

    /* Case: result = (-operand_x) - operand_y */
    if (tmp_x->sign == NEGATIVE && tmp_y->sign == POSITIVE) {
        tmp_x->sign = POSITIVE;
        bigint_addition(result, tmp_x, tmp_y);
        (*result)->sign = NEGATIVE;
        goto RETURN;
    }

    /* Case: operand_x == operand_y */
    if(bigint_compare(tmp_x, tmp_y) == SAME) {
        bigint_set_zero(result);
        goto RETURN;
    }

    /* Case: operand_x > operand_y */
    if (bigint_compare_abs(tmp_x, tmp_y) == LEFT_IS_BIG) 
    {
        bigint_subtraction_unsigned(result, tmp_x, tmp_y);
        
        /* Set the sign of the result */
        if (tmp_x->sign == POSITIVE && tmp_y->sign == POSITIVE) (*result)->sign = POSITIVE;
        if (tmp_x->sign == NEGATIVE && tmp_y->sign == NEGATIVE) (*result)->sign = NEGATIVE;

        goto RETURN;
    }
    else
    {
        bigint_subtraction_unsigned(result, tmp_y, tmp_x);

        /* Set the sign of the result */
        if (tmp_x->sign == POSITIVE && tmp_y->sign == POSITIVE) (*result)->sign = NEGATIVE;
        if (tmp_x->sign == NEGATIVE && tmp_y->sign == NEGATIVE) (*result)->sign = POSITIVE;

        goto RETURN;
    }

    RETURN:

    /* Free allocated memory */
    bigint_delete(&tmp_x);
    bigint_delete(&tmp_y);
}
