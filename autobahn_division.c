#include "autobahn.h"

/**
 * @brief Handles special cases in Bigint division.
 *
 * @param quotient [out] Pointer to the resulting quotient Bigint.
 * @param remainder [out] Pointer to the resulting remainder Bigint.
 * @param dividend [in] Pointer to the dividend Bigint.
 * @param divisor [in] Pointer to the divisor Bigint.
 * @return true if a special case is encountered, false otherwise.
 */
static bool bigint_division_special_case(Bigint **quotient, Bigint **remainder, const Bigint *dividend, const Bigint *divisor)
{
    /* Invalid case: zero divisor */
    if (bigint_is_zero(divisor) == TRUE)
    {
        printf("Error: Divisor must be non-zero.\n"); // Log an error message
        bigint_set_zero(quotient);
        bigint_set_zero(remainder);
        return true;
    }

    /* Invalid case: operands are negative */
    if (divisor->sign == NEGATIVE || dividend->sign == NEGATIVE)
    {
        printf("Error: Divisor and dividend must be positive.\n"); // Log an error message
        bigint_set_zero(quotient);
        bigint_set_zero(remainder);
        return true;
    }

    /* Special case: dividend < divisor, resulting in zero quotient and dividend remainder */
    if (bigint_compare(dividend, divisor) == LEFT_IS_SMALL)
    {
        bigint_set_zero(quotient);
        bigint_copy(remainder, dividend);
        return true;
    }

    /* Special case: divisor is one, resulting in zero remainder and dividend quotient */
    if (bigint_is_one(divisor) == TRUE)
    {
        bigint_copy(quotient, dividend);
        bigint_set_zero(remainder);
        return true;
    }

    /* Division is needed */
    return false;
}

/**
 * @brief Multiplies a Bigint by a single Word.
 *
 * @param result [out] Pointer to the resulting Bigint.
 * @param bigint [in] Pointer to the source Bigint.
 * @param word [in] The Word to multiply with the Bigint.
 */
static void multiplication_bigint_and_word(Bigint **result, const Bigint *bigint, const Word word)
{
    Bigint *word_to_bigint = NULL;

    /* Allocate Bigint */
    bigint_new(&word_to_bigint, 1);

    /* Convert Word to Bigint for compatibility */
    word_to_bigint->digits[0] = word;

    /* Perform multiplication between Bigint and Word */
    bigint_multiplication_textbook(result, bigint, word_to_bigint);

    /* Free allocated memory for Bigint */
    bigint_delete(&word_to_bigint);
}

/**
 * @brief Adds a Word to a Bigint.
 *
 * @param result [out] Pointer to the resulting Bigint.
 * @param bigint [in] Pointer to the source Bigint.
 * @param word [in] The Word to add to the Bigint.
 */
static void addition_bigint_and_word(Bigint **result, const Bigint *bigint, const Word word)
{
    Bigint *word_to_bigint = NULL;

    /* Allocate Bigint */
    bigint_new(&word_to_bigint, 1);

    /* Convert Word to Bigint for compatibility */
    word_to_bigint->digits[0] = word;

    /* Perform addition between Bigint and Word */
    bigint_addition(result, bigint, word_to_bigint);

    /* Free allocated memory for Bigint */
    bigint_delete(&word_to_bigint);
}

/**
 * @brief Performs binary long division to calculate quotient and remainder.
 *
 * @param quotient [out] Pointer to the resulting quotient Bigint.
 * @param remainder [out] Pointer to the resulting remainder Bigint.
 * @param dividend [in] Pointer to the dividend Bigint.
 * @param divisor [in] Pointer to the divisor Bigint.
 */
void bigint_division_binary_long(Bigint **quotient, Bigint **remainder, const Bigint *dividend, const Bigint *divisor)
{
    /* Check for invalid or special cases of division */
    bool special_case_flag = bigint_division_special_case(quotient, remainder, dividend, divisor);
    if (special_case_flag == true)
        return;

    /* Number of digits for quotient and remainder */
    Word size_quotient = dividend->digit_num - divisor->digit_num + 1;
    Word size_remainder = divisor->digit_num;

    /* Allocate Bigint */
    Bigint *tmp_quotient = NULL;  // Resulting quotient
    Bigint *tmp_remainder = NULL; // Resulting remainder
    Bigint *two_power = NULL;     // 2^i, used for obtaining the quotient
    bigint_new(&tmp_quotient, size_quotient);
    bigint_new(&tmp_remainder, size_remainder);
    bigint_new(&two_power, size_quotient);

    /* Initialize Bigint */
    bigint_set_zero(&tmp_quotient);
    bigint_set_zero(&tmp_remainder);

    /* Iteration count */
    Word digit_idx = dividend->digit_num;

    while(digit_idx--)
    {
        Word bit_idx = BITLEN_OF_WORD;

        while(bit_idx--)
        {
            bigint_expand_one_bit(&tmp_remainder, tmp_remainder);                      // R <- 2R
            tmp_remainder->digits[0] += GET_BIT(dividend->digits[digit_idx], bit_idx); // R <- 2R + bit of dividend

            if (bigint_compare(tmp_remainder, divisor) == LEFT_IS_SMALL) // R < B, then no need to subtract B.
                continue;

            bigint_set_one(&two_power); // Initialize for computing 2^i

            bigint_expand(&two_power, two_power, digit_idx);
            for(Word i = 0; i < bit_idx; i++) bigint_expand_one_bit(&two_power, two_power); // 2^i

            bigint_addition(&tmp_quotient, tmp_quotient, two_power);    // Q <- Q + 2^i
            bigint_subtraction(&tmp_remainder, tmp_remainder, divisor); // R <- R - B
        }
    }

    /* Get result */
    bigint_refine(tmp_quotient);
    bigint_refine(tmp_remainder);
    bigint_copy(quotient, tmp_quotient);
    bigint_copy(remainder, tmp_remainder);

    /* Free Bigint */
    bigint_delete(&tmp_quotient);
    bigint_delete(&tmp_remainder);
    bigint_delete(&two_power);
}


/**
 * @brief
 *
 * @param quotient
 * @param remainder
 * @param dividend
 * @param divisor
 */
void bigint_division_naive(Bigint **quotient, Bigint **remainder, const Bigint *dividend, const Bigint *divisor)
{
    /* Check invalid case or special case of division */
    bool special_case_flag = bigint_division_special_case(quotient, remainder, dividend, divisor);
    if (special_case_flag == true)
        return;

    /* Number of digits about quotient and remainder */
    Word size_quotient = dividend->digit_num - divisor->digit_num + 1;
    Word size_remainder = divisor->digit_num;

    /* Allocate Bigint */
    Bigint *tmp_quotient = NULL;  // result of quotient
    Bigint *tmp_remainder = NULL; // result of remainder
    bigint_new(&tmp_quotient, size_quotient);
    bigint_new(&tmp_remainder, size_remainder);

    /* Initialize Bigint */
    bigint_set_zero(&tmp_quotient);
    bigint_copy(&tmp_remainder, dividend);

    /* Naive division */
    while (bigint_compare(tmp_remainder, divisor) != LEFT_IS_SMALL)
    {
        addition_bigint_and_word(&tmp_quotient, tmp_quotient, 1);   // Q <- Q + 1
        bigint_subtraction(&tmp_remainder, tmp_remainder, divisor); // R <- R - B
    }

    /* Get result */
    bigint_refine(tmp_quotient);
    bigint_refine(tmp_remainder);
    bigint_copy(quotient, tmp_quotient);
    bigint_copy(remainder, tmp_remainder);

    /* Free Bigint */
    bigint_delete(&tmp_quotient);
    bigint_delete(&tmp_remainder);
}

/**
 * @brief Computes the quotient of dividing a two-word integer by a one-word integer.
 *
 * @param dividend_high [in] The most significant word of the dividend.
 * @param dividend_low [in] The least significant word of the dividend.
 * @param divisor [in] The divisor.
 * @return Word The quotient of (dividend_high||dividend_low) / divisor.
 */
static Word get_quotient_of_division_two_word(Word dividend_high, Word dividend_low, Word divisor)
{
    Word quotient = 0; // Resulting quotient
    Word remainder = dividend_high;
    Word bit_idx = BITLEN_OF_WORD;

    /* Divide: (dividend_high||dividend_low) / divisor */
    while(bit_idx--)
    {
        Word bit_dividend_low = GET_BIT(dividend_low, bit_idx);       // ai
        Word bit_expanded = (Word)1 << bit_idx;                       // Set correct index

        if (GET_MSB(remainder) == 1) {
            quotient += bit_expanded;                               // Q <- Q + ai
            remainder = remainder * 2 + bit_dividend_low - divisor; // R <- 2R + ai - B
        }
        else {
            remainder = remainder * 2 + bit_dividend_low; // R <- 2R + ai

            if (remainder >= divisor) {
                quotient += bit_expanded;        // Q <- Q + ai
                remainder = remainder - divisor; // R <- R - B
            }
        }
    }

    return quotient;
}

/**
 * @brief Computes the quotient and remainder of dividing a high two-word integer by a one-word integer.
 *
 * @param quotient [out] Pointer to store the quotient.
 * @param remainder [out] Pointer to store the remainder as a Bigint.
 * @param dividend [in] The expanded two-word dividend.
 * @param divisor [in] The divisor.
 */
static void division_expanded_two_word(Word *quotient, Bigint **remainder, const Bigint *dividend, const Bigint *divisor)
{
    Word word_dividend_high = dividend->digits[divisor->digit_num];    // Most significant digit of divisor
    Word word_dividend_low = dividend->digits[divisor->digit_num - 1]; // Second most significant digit of divisor
    Word word_divisor = divisor->digits[divisor->digit_num - 1];       // Second most significant digit of divisor

    /* Compute quotient */
    if (dividend->digit_num == divisor->digit_num)
        *quotient = word_dividend_low / word_divisor;

    /* Compute quotient */
    if (dividend->digit_num == divisor->digit_num + 1)
    {
        if (word_dividend_high == word_divisor)
            *quotient = (Word)(-1); // all bit of quotient is 1
        else
            *quotient = get_quotient_of_division_two_word(word_dividend_high, word_dividend_low, word_divisor);
    }

    /* Compute remainder */
    multiplication_bigint_and_word(remainder, divisor, *quotient); // R <- BQ
    bigint_subtraction(remainder, dividend, *remainder);           // R <- A - BQ

    /* Final computation since R can be negative */
    while ((*remainder)->sign == NEGATIVE) // if (R < 0)
    {
        *quotient = *quotient - 1;                       // Q <- Q - 1
        bigint_addition(remainder, *remainder, divisor); // R <- R + B
    }
}

/**
 * @brief Divides a two-word integer by a one-word integer, computing the quotient and remainder.
 *
 * @param quotient [out] Pointer to store the quotient.
 * @param remainder [out] Pointer to store the remainder as a Bigint.
 * @param dividend [in] The dividend.
 * @param divisor [in] The divisor.
 */
void division_two_word(Word *quotient, Bigint **remainder, const Bigint *dividend, const Bigint *divisor)
{
    /* Special Case: dividend < divisor */
    if (bigint_compare(dividend, divisor) == LEFT_IS_SMALL)
    {
        *quotient = 0;
        bigint_copy(remainder, dividend);
        return;
    }

    Word shift_bit_count = 0;
    Word most_significant_word_of_divisor = divisor->digits[divisor->digit_num - 1];

    Bigint *dividend_expand = NULL;
    Bigint *divisor_expand = NULL;

    /* Allocate Bigint */
    bigint_new(&dividend_expand, dividend->digit_num);
    bigint_new(&divisor_expand, divisor->digit_num);

    /* Initialization */
    bigint_copy(&dividend_expand, dividend);
    bigint_copy(&divisor_expand, divisor);

    /* Expand */
    while (GET_MSB(most_significant_word_of_divisor) != 1)
    {
        bigint_expand_one_bit(&dividend_expand, dividend_expand);
        bigint_expand_one_bit(&divisor_expand, divisor_expand);

        most_significant_word_of_divisor <<= 1;
        shift_bit_count++;
    }

    /* Divide */
    division_expanded_two_word(quotient, remainder, dividend_expand, divisor_expand);

    /* Compress */
    while (shift_bit_count--)
        bigint_compress_one_bit(remainder, *remainder);

    /* Free Bigint */
    bigint_delete(&dividend_expand);
    bigint_delete(&divisor_expand);
}


/**
 * @brief Computing the quotient and remainder by word long division algorithm.
 *
 * @param quotient [out] Pointer to store the quotient.
 * @param remainder [out] Pointer to store the remainder as a Bigint.
 * @param dividend [in] The long dividend as a Bigint.
 * @param divisor [in] The divisor.
 */
void bigint_division_word_long(Bigint **quotient, Bigint **remainder, const Bigint *dividend, const Bigint *divisor)
{
    /* Check invalid cases or special cases of division */
    bool special_case_flag = bigint_division_special_case(quotient, remainder, dividend, divisor);
    if (special_case_flag == true)
        return;

    /* Number of digits for quotient and remainder */
    Word size_quotient = dividend->digit_num - divisor->digit_num + 1;
    Word size_remainder = divisor->digit_num;

    Bigint *tmp_quotient = NULL;  // Resulting quotient
    Bigint *tmp_remainder = NULL; // Resulting remainder
    Bigint *rw_plus_a = NULL;     // Intermediate value to compute the remainder

    /* Allocate Bigint */
    bigint_new(&tmp_quotient, size_quotient);
    bigint_new(&tmp_remainder, size_remainder);
    bigint_new(&rw_plus_a, size_remainder + 1);

    /* Initialization */
    bigint_set_zero(&tmp_quotient);
    bigint_set_zero(&tmp_remainder);

    /* Compute word-long division */
    for (int digit_idx = dividend->digit_num - 1; digit_idx >= 0; digit_idx--)
    {
        Word word_dividend = dividend->digits[digit_idx]; // Ai
        Word word_quotient = 0;                           // Qi

        bigint_expand(&rw_plus_a, tmp_remainder, 1);                           // T <- RW
        addition_bigint_and_word(&rw_plus_a, rw_plus_a, word_dividend);        // T <- RW + Ai
        division_two_word(&word_quotient, &tmp_remainder, rw_plus_a, divisor); // (Qi, R) <- T / B
        bigint_expand(&tmp_quotient, tmp_quotient, 1);                         // Q <- QW
        addition_bigint_and_word(&tmp_quotient, tmp_quotient, word_quotient);  // Q <- QW + Qi
    }

    /* Get the result */
    bigint_refine(tmp_quotient);
    bigint_refine(tmp_remainder);
    bigint_copy(quotient, tmp_quotient);
    bigint_copy(remainder, tmp_remainder);

    /* Free Bigint */
    bigint_delete(&tmp_quotient);
    bigint_delete(&tmp_remainder);
    bigint_delete(&rw_plus_a);
}
