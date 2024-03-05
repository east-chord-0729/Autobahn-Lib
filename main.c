#include "autobahn.h"

/* sample code list */
void addition_and_subtraction()
{
    printf("Sample Code of Addition and Subtraction...\n");

    /* Init */
    Bigint *operand_x = NULL;
    Bigint *operand_y = NULL;
    Bigint *result = NULL;

    /* allocate */
    bigint_new(&operand_x, 1);
    bigint_new(&operand_y, 1);
    bigint_new(&result, 1);

    /* set operands */
    bigint_set_by_hex_string(&operand_x, "c08d7139ec42a07d368e3fdfcc11a53fa2297928fef126d0ae89d501bd16022e", POSITIVE);
    bigint_set_by_hex_string(&operand_y, "d07acf4d5c297f7002fa85004dce801d40c8cdc1bbe5071e4", POSITIVE);

    /* show operands */
    printf("operand_x : ");
    bigint_show_hex(operand_x);
    printf("operand_y : ");
    bigint_show_hex(operand_y);

    /* addition */
    bigint_addition(&result, operand_x, operand_y);

    /* show result */
    printf("result addition    : ");
    bigint_show_hex(result);

    /* subtraction */
    bigint_subtraction(&result, operand_x, operand_y);

    /* show result */
    printf("result subtraction : ");
    bigint_show_hex(result);

    /* free memory */
    bigint_delete(&operand_x);
    bigint_delete(&operand_y);
    bigint_delete(&result);

    printf("\n");
}
void multiplication()
{
    printf("Sample Code of Multiplication...\n");

    /* Init */
    Bigint *operand_x = NULL;
    Bigint *operand_y = NULL;
    Bigint *result = NULL;

    /* allocate */
    bigint_new(&operand_x, 1);
    bigint_new(&operand_y, 1);
    bigint_new(&result, 1);

    /* set operands */
    bigint_set_by_hex_string(&operand_x, "c08d7139ec42a07d368e3fdfcc11a53fa2297928fef126d0ae89d501bd16022e", POSITIVE);
    bigint_set_by_hex_string(&operand_y, "d07acf4d5c297f7002fa85004dce801d40c8cdc1bbe5071e4", POSITIVE);

    /* show operands */
    printf("operand_x : ");
    bigint_show_hex(operand_x);
    printf("operand_y : ");
    bigint_show_hex(operand_y);

    /* multiplication */
    bigint_multiplication_textbook(&result, operand_x, operand_y);

    /* show result */
    printf("result textbook  : ");
    bigint_show_hex(result);

    bigint_multiplication_karatsuba(&result, operand_x, operand_y);

    /* show result */
    printf("result karatsuba : ");
    bigint_show_hex(result);

    /* free memory */
    bigint_delete(&operand_x);
    bigint_delete(&operand_y);
    bigint_delete(&result);

    printf("\n");
}
void squaring()
{
    printf("Sample Code of Squaring...\n");

    /* Init */
    Bigint *operand_x = NULL;
    Bigint *result = NULL;

    /* allocate */
    bigint_new(&operand_x, 1);
    bigint_new(&result, 1);

    /* set operands */
    bigint_set_by_hex_string(&operand_x, "c08d7139ec42a07d368e3fdfcc11a53fa2297928fef126d0ae89d501bd16022e", POSITIVE);

    /* show operands */
    printf("operand_x : ");
    bigint_show_hex(operand_x);

    /* textbook squaring */
    bigint_squaring_textbook(&result, operand_x);

    /* show result */
    printf("result textbook  : ");
    bigint_show_hex(result);

    /* karatsuba squaring */
    bigint_squaring_karatsuba(&result, operand_x);

    /* show result */
    printf("result karatsuba : ");
    bigint_show_hex(result);

    /* free memory */
    bigint_delete(&operand_x);
    bigint_delete(&result);

    printf("\n");
}
void division()
{
    printf("Sample Code of Division...\n");

    /* Init */
    Bigint *dividend = NULL;
    Bigint *divisor = NULL;
    Bigint *quotient = NULL;
    Bigint *remainder = NULL;

    /* allocate */
    bigint_new(&dividend, 1);
    bigint_new(&divisor, 1);
    bigint_new(&quotient, 1);
    bigint_new(&remainder, 1);

    /* set operands */
    bigint_set_by_hex_string(&dividend, "c08d7139ec42a07d368e3fdfcc11a53fa2297928fef126d0ae89d501bd16022e", POSITIVE);
    bigint_set_by_hex_string(&divisor, "d07acf4d5c297f7002fa85004dce801d40c8cdc1bbe5071e4", POSITIVE);

    /* show operands */
    printf("dividend : ");
    bigint_show_hex(dividend);
    printf("divisor : ");
    bigint_show_hex(divisor);

    /* binary long division */
    bigint_division_binary_long(&quotient, &remainder, dividend, divisor);

    /* show result */
    printf("binary long quotient  : ");
    bigint_show_hex(quotient);
    printf("binary long remainder : ");
    bigint_show_hex(remainder);

    /* word long division */
    bigint_division_word_long(&quotient, &remainder, dividend, divisor);

    /* show result */
    printf("  word long quotient  : ");
    bigint_show_hex(quotient);
    printf("  word long remainder : ");
    bigint_show_hex(remainder);

    /* free memory */
    bigint_delete(&dividend);
    bigint_delete(&divisor);
    bigint_delete(&quotient);
    bigint_delete(&remainder);

    printf("\n");
}
void reduction()
{
    printf("Sample Code of Reduction...\n");

    /* Init */
    Bigint *dividend = NULL;
    Bigint *modular = NULL;
    Bigint *barrett_pre_compute = NULL;
    Bigint *result = NULL;

    /* allocate */
    bigint_new(&dividend, 1);
    bigint_new(&modular, 1);
    bigint_new(&barrett_pre_compute, 1);
    bigint_new(&result, 1);

    /* set operands */
    bigint_set_by_hex_string(&dividend, "c08d7139ec42a07d368e3fdfcc11a53fa2297928fef126d0ae89d501bd16022e", POSITIVE);
    bigint_set_by_hex_string(&modular, "d07acf4d5c297f7002fa85004dce801d40c8cdc1bbe5071e4", POSITIVE);

    /* show operands */
    printf("dividend : ");
    bigint_show_hex(dividend);
    printf("modular : ");
    bigint_show_hex(modular);

    /* Pre compute for barrett */
    bigint_reduction_barrett_pre_computed(&barrett_pre_compute, modular);

    /* Barrett reduction */
    bigint_reduction_barrett(&result, dividend, modular, barrett_pre_compute);

    /* show result */
    printf("result : ");
    bigint_show_hex(result);

    /* free memory */
    bigint_delete(&dividend);
    bigint_delete(&modular);
    bigint_delete(&barrett_pre_compute);
    bigint_delete(&result);

    printf("\n");
}
void modular_exponentiation()
{
    printf("Sample Code of Modular Exponentiation...\n");

    /* Init */
    Bigint *base = NULL;
    Bigint *exponent = NULL;
    Bigint *modular = NULL;
    Bigint *result = NULL;

    /* allocate */
    bigint_new(&base, 1);
    bigint_new(&exponent, 1);
    bigint_new(&modular, 1);
    bigint_new(&result, 1);

    /* set operands */
    bigint_set_by_hex_string(&base, "c08d7139ec42a07d368e3fdfcc11a53fa2297928fef126d0ae89d501bd16022e", POSITIVE);
    bigint_set_by_hex_string(&exponent, "d07acf4d5c297f7002fa85004dce801d40c8cdc1bbe5071e4", POSITIVE);
    bigint_set_by_hex_string(&modular, "ac077f929024783a922982b34ae144dde21d1903b68cb1dc43b296274c0b21bb", POSITIVE);

    /* show operands */
    printf("base     : ");
    bigint_show_hex(base);
    printf("exponent : ");
    bigint_show_hex(exponent);
    printf("modular  : ");
    bigint_show_hex(modular);

    /* Pre compute for barrett */
    bigint_exponentiation_modular_left_to_right(&result, base, exponent, modular);

    /* show result */
    printf("result left to right : ");
    bigint_show_hex(result);

    /* Barrett reduction */
    bigint_exponentiation_modular_montgomery_ladder(&result, base, exponent, modular);

    /* show result */
    printf("   result montgomery : ");
    bigint_show_hex(result);

    /* free memory */
    bigint_delete(&base);
    bigint_delete(&exponent);
    bigint_delete(&modular);
    bigint_delete(&result);

    printf("\n");
}

int main()
{
    /* Sample Code */
    addition_and_subtraction();
    multiplication();
    squaring();
    division();
    reduction();
    modular_exponentiation();

    return 0;
}