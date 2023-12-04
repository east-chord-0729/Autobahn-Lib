#include "autobahn_common.h"

#pragma warning(disable: 28182)
#pragma warning(disable: 6308)

/**
 * @brief Allocates memory for a new Bigint.
 * 
 * @param bigint [out] Pointer to the Bigint.
 * @param new_digit_num [in] Number of digits for the new Bigint.
 */
void bigint_new(Bigint** bigint, Word new_digit_num)
{
    /* Free allocated memory */
    if (*bigint != NULL) 
        bigint_delete(bigint);

    /* Size is at least 1 */
    if (new_digit_num == 0) 
        new_digit_num = 1;

    /* Allocate Bigint */
    *bigint = (Bigint*)malloc(SIZE_OF_BIGINT);
    (*bigint)->sign = POSITIVE;
    (*bigint)->digit_num = new_digit_num;
    (*bigint)->digits = (Word*)calloc(new_digit_num, SIZE_OF_WORD);
}

/**
 * @brief Deallocates memory for a Bigint.
 * 
 * @param bigint [in, out] Pointer to the Bigint.
 */
void bigint_delete(Bigint** bigint)
{
    /* Invalid pointer */
    if (*bigint == NULL) 
        return;

    /* Free memory */
    free((*bigint)->digits);
    free(*bigint);
}

/**
 * @brief Refines the memory allocated for a Bigint.
 * 
 * @param bigint [in, out] Pointer to the Bigint.
 */
void bigint_refine(Bigint* bigint)
{
    /* Invalid pointer */
    if (bigint == NULL) 
        return;

    /* New number of digits */
    Word new_digit_num = bigint->digit_num;

    /* Calculate the new number of digits */
    while (new_digit_num > 1) {
        if (bigint->digits[new_digit_num - 1] != 0) 
            break; // Stop if an integer is found from the top
        new_digit_num--;
    }

    /* Check if refinement is needed */
    if (bigint->digit_num == new_digit_num) 
        return;

    /* Refine memory */
    bigint->digit_num = new_digit_num;
    bigint->digits = (Word*)realloc(bigint->digits, SIZE_OF_WORD * new_digit_num);

    /* Zero is always positive */
    if (bigint_is_zero(bigint)) 
        bigint->sign = POSITIVE;
}

/**
 * @brief Sets the value of a Bigint from an array of Words.
 * 
 * @param bigint [out] Pointer to the destination Bigint.
 * @param array [in] Array of Words representing the value.
 * @param sign [in] Sign of the value.
 * @param digit_num [in] Number of digits.
 */
void bigint_set_by_array(Bigint** bigint, const Word* array, Sign sign, Word digit_num)
{
    /* Allocate Bigint */
    bigint_new(bigint, digit_num); 
    
    /* Set sign */
    (*bigint)->sign = sign; 
    
    /* Copy */
    for (int digit_idx = 0; digit_idx < digit_num; digit_idx++) 
        (*bigint)->digits[digit_idx] = array[digit_idx];
}

/**
 * @brief Sets the value of a Bigint from a hexadecimal string.
 * 
 * @param bigint [out] Pointer to the destination Bigint.
 * @param string [in] Hexadecimal string representing the value.
 * @param sign [in] Sign of the value.
 */
void bigint_set_by_hex_string(Bigint** bigint, const char* string, Sign sign)
{
    /* Get length of string and new digit number */
    int length_string = (int)strlen(string);
    Word new_digit_num = (length_string / (SIZE_OF_WORD * 2)) + (length_string % (SIZE_OF_WORD * 2) != 0);

    /* Error check : string is not hex format */
    for (int str_idx = 0; str_idx < length_string; str_idx++) 
    {
	    if (!(('0'<=string[str_idx] && string[str_idx] <='9') || 
              ('a'<=string[str_idx] && string[str_idx] <='f') || 
              ('A'<=string[str_idx] && string[str_idx] <='F'))) {
		    printf("%d-th character is invalid. Do not use %c.", str_idx, string[str_idx]);
		    exit(1);
	    }
    }

    /* Allocate Bigint */
    bigint_new(bigint, new_digit_num);
    (*bigint)->sign = sign;

    /* String to Bigint */
    for (Word idx = 0; idx < new_digit_num; idx++) 
    {
        /* Char to int */
        for (Word idx_4bit = 0; idx_4bit < BITLEN_OF_WORD; idx_4bit += 4)
        {
            char char_1byte = string[--length_string];
            Word int_4bit = 0;

            /* One byte char to 4-bits int */
            if ('0' <= char_1byte && char_1byte <= '9') 
                int_4bit = char_1byte - '0';
            if ('A' <= char_1byte && char_1byte <= 'F') 
                int_4bit = char_1byte - 'A' + 10;
            if ('a' <= char_1byte && char_1byte <= 'f') 
                int_4bit = char_1byte - 'a' + 10;

            /* Fill in the digit */
            (*bigint)->digits[idx] += int_4bit << idx_4bit;

            /* String is gone */
            if (length_string == 0) 
                break;
        }
    }

    /* Free unused memory */
    bigint_refine(*bigint);
}

/**
 * @brief Copies the value of one Bigint to another.
 * 
 * @param bigint_dest [out] Pointer to the destination Bigint.
 * @param bigint_src [in] Pointer to the source Bigint.
 */
void bigint_copy(Bigint** bigint_dest, const Bigint* bigint_src)
{ 
    /* Allocate new Bigint */
    int new_digit_num = bigint_src->digit_num;
    bigint_new(bigint_dest, new_digit_num);

    /* Copy the digits */
    for (int idx = 0; idx < new_digit_num; idx++)
        (*bigint_dest)->digits[idx] = bigint_src->digits[idx];

    /* Copy the sign */
    (*bigint_dest)->sign = bigint_src->sign;
}

/**
 * @brief Copies a part of a Bigint to a new Bigint.
 * 
 * @param result [out] Pointer to the resulting Bigint.
 * @param bigint [in] Pointer to the source Bigint.
 * @param offset_start [in] Starting offset.
 * @param offset_end [in] Ending offset.
 */
void bigint_copy_part(Bigint** result, const Bigint* bigint, Word offset_start, Word offset_end) 
{
    /* Invalid offset */
	if (bigint->digit_num < offset_end || bigint->digit_num < offset_start || offset_end < offset_start) {
        printf("invalid offset\n");
        bigint_copy(result, bigint);
        return;
    }

    /* Allocate Bigint */
    Word new_digit_num = offset_end - offset_start;
    bigint_new(result, new_digit_num);

    /* copy the digits */
    for(int i = 0; i < new_digit_num; i++)
        (*result)->digits[i] = bigint->digits[offset_start + i];

    bigint_refine(*result);
}

/**
 * @brief Expands the size of a Bigint by a given number of words.
 * 
 * @param result [out] Pointer to the resulting Bigint.
 * @param bigint [in] Pointer to the source Bigint.
 * @param wordlen [in] Number of words to expand.
 */
void bigint_expand(Bigint** result, const Bigint* bigint, Word wordlen) 
{
    Bigint* tmp_result = NULL;
    int count = bigint->digit_num;

    /* Allocate Bigint */
    bigint_new(&tmp_result, bigint->digit_num + wordlen);

    /* Expand (left shift) wordlen index */
    while(count--)
        tmp_result->digits[count + wordlen] = bigint->digits[count];

    /* Get Result */
    bigint_refine(tmp_result);
    bigint_copy(result, tmp_result);

    /* Free Bigint */
    bigint_delete(&tmp_result);
}

/**
 * @brief Compresses the size of a Bigint by a given number of words.
 * 
 * @param result [out] Pointer to the resulting Bigint.
 * @param bigint [in] Pointer to the source Bigint.
 * @param wordlen [in] Number of words to compress.
 */
void bigint_compress(Bigint** result, const Bigint* bigint, Word wordlen) 
{
    /* Over compress */
    if (bigint->digit_num <= wordlen) {
        bigint_set_zero(result);
        return;
    }

    /* No need to compress */
    if (wordlen == 0) {
        bigint_copy(result, bigint);
        return;
    }

    Bigint* tmp_result = NULL;
    Word count = bigint->digit_num - wordlen;

    /* Allocate Bigint */
    bigint_new(&tmp_result, bigint->digit_num);

    /* Compress (shift right) wordlen index */
    while(count--)
        tmp_result->digits[count] = bigint->digits[count + wordlen];

    /* Get result */
    bigint_refine(tmp_result);
    bigint_copy(result, tmp_result);

    /* Free Bigint */
    bigint_delete(&tmp_result);
}

/**
 * @brief Expands the size of a Bigint by one bit, shifting digits left.
 * 
 * @param result [out] Pointer to the resulting Bigint.
 * @param bigint [in] Pointer to the source Bigint.
 */
void bigint_expand_one_bit(Bigint** result, const Bigint* bigint)
{
    Word carry = 0;
    Bigint* tmp_result = NULL;

    /* Allocate Bigint */
    bigint_new(&tmp_result, bigint->digit_num + 1);

    /* One bit left shift */
    for (int idx = 0; idx < bigint->digit_num; idx++) {
        tmp_result->digits[idx] = (bigint->digits[idx] << 1) + carry;
        carry = (bigint->digits[idx] >> (BITLEN_OF_WORD - 1)) & MASK1BIT;
    }

    /* Final carry */
    tmp_result->digits[bigint->digit_num] = carry;

    /* Get result */
    bigint_refine(tmp_result);
    bigint_copy(result, tmp_result);

    /* Free Bigint */
    bigint_delete(&tmp_result);
}

/**
 * @brief Compresses the size of a Bigint by one bit, shifting digits right.
 * 
 * @param result [out] Pointer to the resulting Bigint.
 * @param bigint [in] Pointer to the source Bigint.
 */
void bigint_compress_one_bit(Bigint** result, const Bigint* bigint)
{
    Word carry = 0;
    Bigint* tmp_result = NULL;

    /* Allocate Bigint */
    bigint_new(&tmp_result, bigint->digit_num);

    /* One bit right shift */
    for (int idx = bigint->digit_num - 1; idx >= 0; idx--) {
        tmp_result->digits[idx] = (bigint->digits[idx] >> 1) + carry;
        carry = (bigint->digits[idx] & MASK1BIT) << (BITLEN_OF_WORD - 1);
    }

    /* Get result */
    bigint_refine(tmp_result);
    bigint_copy(result, tmp_result);

    /* Free Bigint */
    bigint_delete(&tmp_result);
}

/**
 * @brief Returns the total bit length of a Bigint.
 * 
 * @param bigint [in] Pointer to the Bigint.
 * @return Word Total bit length of the Bigint.
 */
int bigint_get_bit_length(const Bigint* bigint)
{
    return bigint->digit_num * BITLEN_OF_WORD;
}

/**
 * @brief Returns the value of a specific bit in a Bigint.
 * 
 * @param bigint [in] Pointer to the Bigint.
 * @param bit_idx [in] Index of the bit to retrieve.
 * @return Word Value of the specified bit (1 or 0).
 */
Word bigint_get_bit(const Bigint* bigint, Word digit_idx, Word bit_idx)
{
    /* is bit 1? or 0? */
    Word bit = (bigint->digits[digit_idx] >> bit_idx) & MASK1BIT;

    return bit;
}

/**
 * @brief Sets the value of a Bigint to zero.
 * 
 * @param bigint [out] Pointer to the Bigint.
 */
void bigint_set_zero(Bigint** bigint)
{
    bigint_new(bigint, 1);

    (*bigint)->sign = POSITIVE;
    (*bigint)->digits[0] = 0;
}

/**
 * @brief Sets the value of a Bigint to one.
 * 
 * @param bigint [out] Pointer to the Bigint.
 */
void bigint_set_one(Bigint** bigint)
{
    bigint_new(bigint, 1);

    (*bigint)->sign = POSITIVE;
    (*bigint)->digits[0] = 1;
}

/**
 * @brief Checks if a Bigint is equal to zero.
 * 
 * @param bigint [in] Pointer to the Bigint.
 * @return char TRUE if the Bigint is zero, FALSE otherwise.
 */
char bigint_is_zero(const Bigint* bigint)
{
    return (bigint->digit_num != 1 || bigint->digits[0] != 0) ? FALSE : TRUE;
}

/**
 * @brief Checks if a Bigint is equal to one.
 * 
 * @param bigint [in] Pointer to the Bigint.
 * @return char TRUE if the Bigint is one, FALSE otherwise.
 */
char bigint_is_one(const Bigint* bigint)
{
    return (bigint->digit_num != 1 || bigint->digits[0] != 1) ? FALSE : TRUE;
}

/**
 * @brief Compares the absolute values of two Bigints.
 * 
 * @param operand_x [in] Pointer to the first Bigint.
 * @param operand_y [in] Pointer to the second Bigint.
 * @return char -1 if operand_x < operand_y, 0 if equal, 1 if operand_x > operand_y.
 */
char bigint_compare_abs(const Bigint* operand_x, const Bigint* operand_y)
{
    // Return -1 if operand_x's digit_num is less than operand_y's digit_num
    if (operand_x->digit_num < operand_y->digit_num) return -1;
    // Return 1 if operand_x's digit_num is greater than operand_y's digit_num
    if (operand_x->digit_num > operand_y->digit_num) return 1;

    // If digit_nums are equal, compare each digit from most significant to least significant
    for (int idx = operand_x->digit_num - 1; idx >= 0; idx--) {
        // Return -1 if the digit in operand_x is less than the digit in operand_y
        if (operand_x->digits[idx] < operand_y->digits[idx]) return -1;
        // Return 1 if the digit in operand_x is greater than the digit in operand_y
        if (operand_x->digits[idx] > operand_y->digits[idx]) return 1;
    }

    // If all digits are equal, return 0
    return 0;
}

/**
 * @brief Compares two Bigints, considering sign.
 * 
 * @param operand_x [in] Pointer to the first Bigint.
 * @param operand_y [in] Pointer to the second Bigint.
 * @return char -1 if operand_x < operand_y, 0 if equal, 1 if operand_x > operand_y.
 */
char bigint_compare(const Bigint* operand_x, const Bigint* operand_y)
{
    // Return -1 if operand_x is negative and operand_y is positive
    if (operand_x->sign == NEGATIVE && operand_y->sign == POSITIVE) return -1;
    // Return 1 if operand_x is positive and operand_y is negative
    if (operand_x->sign == POSITIVE && operand_y->sign == NEGATIVE) return 1;

    // Compare the absolute values of operand_x and operand_y
    char abs_comparison = bigint_compare_abs(operand_x, operand_y);

    // Adjust the result based on the signs of operand_x and operand_y
    if (operand_x->sign == NEGATIVE && operand_y->sign == POSITIVE) return -abs_comparison;
    if (operand_x->sign == POSITIVE && operand_y->sign == NEGATIVE) return abs_comparison;

    return abs_comparison;
}

/**
 * @brief Generates a random Bigint with the specified sign and digit number.
 * 
 * @param bigint [out] Pointer to the generated Bigint.
 * @param sign [in] Sign of the generated Bigint (POSITIVE or NEGATIVE).
 * @param digit_num [in] Number of digits in the generated Bigint.
 */
void bigint_generate_random_number(Bigint** bigint, Sign sign, Word digit_num)
{
    /* Allocate memory for Bigint */
    bigint_new(bigint, digit_num);
    (*bigint)->sign = sign;

    /* Variables for generating a random number */
    unsigned char* byte_rand_num = (unsigned char*)(*bigint)->digits; // Pointer to the bytes of the bigint.
    Word count = digit_num * SIZE_OF_WORD;                 // Number of bytes in the bigint.

    /* 8-bit random number generator */
    while (count--)
        *byte_rand_num++ = (Word)rand() & MASK8BIT;

    /* Free unused memory */
    bigint_refine(*bigint);
}

/**
 * @brief Displays the hexadecimal representation of a Bigint.
 * 
 * @param bigint [in] Pointer to the Bigint to be displayed.
 */
void bigint_show_hex(const Bigint* bigint) 
{
    /* Display negative sign if present */
    if (bigint->sign) printf("-");

    /* Print the most significant digit */
    printf(PRINT_WORD_FORMAT, (Word)bigint->digits[bigint->digit_num - 1]);

    /* Print the remaining digits */
    for (int idx = bigint->digit_num - 2; idx >= 0; idx--)
        printf(PRINT_WORD_FIX_FORMAT, (Word)bigint->digits[idx]);

    /* Line break for better readability */
    printf("\n");
}