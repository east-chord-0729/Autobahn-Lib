#ifndef AUTOBAHN_H
#define AUTOBAHN_H

#include "autobahn_common.h"

/** @brief Addition and Subtraction */
void bigint_addition    (Bigint** result, const Bigint* operand_x, const Bigint* operand_y);
void bigint_subtraction (Bigint** result, const Bigint* operand_x, const Bigint* operand_y);

/** @brief Multiplication */
void bigint_multiplication_textbook  (Bigint** result, const Bigint* operand_x, const Bigint* operand_y);
void bigint_multiplication_karatsuba (Bigint** result, const Bigint* operand_x, const Bigint* operand_y);

/** @brief Squaring */
void bigint_squaring_textbook  (Bigint** result, const Bigint* operand_x);
void bigint_squaring_karatsuba (Bigint** result, Bigint* operand_x);

/** @brief Division */
void bigint_division_word_long   (Bigint** quotient, Bigint** remainder, const Bigint* dividend, const Bigint* divisor);
void bigint_division_binary_long (Bigint** quotient, Bigint** remainder, const Bigint* dividend, const Bigint* divisor);
void bigint_division_naive       (Bigint** quotient, Bigint** remainder, const Bigint* dividend, const Bigint* divisor); // DO NOT USE (SLOW)

/** @brief Reduction */
void bigint_reduction_barrett_pre_computed (Bigint** barrett_pre_computed, const Bigint* modular);
void bigint_reduction_barrett              (Bigint** result, const Bigint* bigint, const Bigint* modular, const Bigint* pre_computed);

/** @brief Exponentiation */
void bigint_exponentiation_modular_left_to_right     (Bigint** result, const Bigint* base, const Bigint* exponent, const Bigint* modular);
void bigint_exponentiation_modular_montgomery_ladder (Bigint** result, const Bigint* base, const Bigint* exponent, const Bigint* modular);

#endif