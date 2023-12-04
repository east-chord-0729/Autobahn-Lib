/**
 * @file autobahn_common.h
 * @author Your Name (you@domain.com)
 * @brief Common header file for the Autobahn library.
 * @version 0.1
 * @date 2023-11-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef AUTOBAHN_COMMON_H
#define AUTOBAHN_COMMON_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define BI_WORD32

/* Define word size */
#if defined(BI_WORD8)
    typedef uint8_t Word;
    #define PRINT_WORD_FORMAT "%x"
    #define PRINT_WORD_FIX_FORMAT "%02x"
#elif defined(BI_WORD64)
    typedef uint64_t Word;
    #define PRINT_WORD_FORMAT "%llx"
    #define PRINT_WORD_FIX_FORMAT "%016llx"
#else
    typedef uint32_t Word;
    #define PRINT_WORD_FORMAT "%x"
    #define PRINT_WORD_FIX_FORMAT "%08x"
#endif

#define SIZE_OF_WORD sizeof(Word)
#define SIZE_OF_BIGINT sizeof(Bigint)
#define BITLEN_OF_WORD (sizeof(Word) * 8)
#define MASK1BIT 0x01
#define MASK8BIT 0xFF
#define SAME 0
#define LEFT_IS_BIG 1
#define LEFT_IS_SMALL -1
#define FALSE 0
#define TRUE 1

/** @brief Enumeration representing the sign of a big integer. */
typedef enum {
    POSITIVE = 0,
    NEGATIVE = 1
} Sign;

/** @brief Structure representing a big integer. */
typedef struct {
    Sign sign;         /**< Sign of the big integer. */
    Word digit_num;    /**< Number of digits in the big integer. */
    Word* digits;      /**< Array of digits representing the big integer. */
} Bigint;

/** @brief Memory Control */
void bigint_new    (Bigint** bigint, Word new_digit_num); /**< Allocates memory for a new Bigint. */
void bigint_delete (Bigint** bigint);                     /**< Deallocates memory for a Bigint. */
void bigint_refine (Bigint* bigint);                      /**< Refines the memory allocated for a Bigint. */

/** @brief Set or Copy */
void bigint_set_by_array      (Bigint** bigint, const Word* array, Sign sign, Word digit_num); /**< Sets the value of a Bigint from an array of Words. */
void bigint_set_by_hex_string (Bigint** bigint, const char* string, Sign sign);                /**< Sets the value of a Bigint from a hexadecimal string. */
void bigint_copy              (Bigint** bigint_dest, const Bigint* bigint_src);                /**< Copies the value of one Bigint to another. */
void bigint_copy_part         (Bigint** result, const Bigint* bigint, Word offset_start, Word offset_end); /**< Copies a part of a Bigint to a new Bigint. */

/** @brief Left or right shift */
void bigint_expand           (Bigint** result, const Bigint* bigint, Word wordlen); /**< Sets the value of a Bigint to zero. */
void bigint_compress         (Bigint** result, const Bigint* bigint, Word wordlen); /**< Compresses the size of a Bigint by a given number of words. */
void bigint_expand_one_bit   (Bigint** result, const Bigint* bigint);               /**< Expands the size of a Bigint by one bit, shifting digits left. */
void bigint_compress_one_bit (Bigint** result, const Bigint* bigint);               /**< Compresses the size of a Bigint by one bit, shifting digits right. */

/** @brief Bit operation */
#define GET_MSB(word) (((word) >> (BITLEN_OF_WORD - 1)) & MASK1BIT) /**< Get most significant bit of word. */
#define GET_BIT(word, bit_idx) (((word) >> (bit_idx)) & MASK1BIT)   /**< Get i-th bit of word */

/** @brief One or Zero operation */
void bigint_set_zero (Bigint** bigint);       /**< Sets the value of a Bigint to zero. */
void bigint_set_one  (Bigint** bigint);       /**< Sets the value of a Bigint to one. */
char bigint_is_zero   (const Bigint* bigint); /**< Checks if a Bigint is equal to zero. */
char bigint_is_one    (const Bigint* bigint); /**< Checks if a Bigint is equal to one. */

/** @brief Etc. */
char bigint_compare     (const Bigint* operand_x, const Bigint* operand_y);      /**< Compares the absolute values of two Bigints. */
char bigint_compare_abs (const Bigint* operand_x, const Bigint* operand_y);      /**< Compares two Bigints, considering sign. */
void bigint_generate_random_number(Bigint** bigint, Sign sign, Word digit_num);  /**< Generates a random Bigint with the specified sign and digit number. */
void bigint_show_hex    (const Bigint* bigint); /**< Displays the hexadecimal representation of a Bigint. */
#endif
