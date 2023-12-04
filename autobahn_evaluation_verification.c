#include "autobahn_evaluation.h"

/**
 * @brief read big integer from file.
 * 
 * @param bigint bigint pointer.
 * @param file bigint file.
 */
static void bigint_read(Bigint** bigint, FILE* file)
{
    char buffer[513] = {0}; //! change 65 if 8-bits. change 513 if 32,64-bits.
    fscanf(file, "%s", buffer);
   	bigint_set_by_hex_string(bigint, buffer, POSITIVE);
}

/**
 * @brief verification code using random value
 * 
 */
void bigint_random_test()
{
    /* file open */
    FILE* file_x = fopen("test/random_test_vectors/operand_x.txt", "r"); //! change operand_x8.txt if 8-bits.
    FILE* file_y = fopen("test/random_test_vectors/operand_y.txt", "r"); //! change operand_y8.txt if 8-bits.
    FILE* file_z = fopen("test/random_test_vectors/operand_z.txt", "r"); //! change operand_z8.txt if 8-bits.

    /* file open error */
    if(file_x == NULL || file_y == NULL || file_z == NULL) {
        perror("bigint_test: file open error");
        exit(1);
    }

    /* Init */
    Bigint *operand_x = NULL;
    Bigint *operand_y = NULL;
    Bigint *operand_z = NULL;
    Bigint *result = NULL;
    Bigint *result2 = NULL;

    /* allocate */
    bigint_new(&operand_x, 1);
    bigint_new(&operand_y, 1);
    bigint_new(&operand_z, 1);
    bigint_new(&result, 1);
    bigint_new(&result2, 1);

    /* Test start */
    for(int i = 0; i < 1000; i++)
    {
        /* read */
        bigint_read(&operand_x, file_x);
        bigint_read(&operand_y, file_y);
        bigint_read(&operand_z, file_z);

        /* operation */
        bigint_addition(&result, operand_x, operand_y);
        // bigint_subtraction(&result, operand_x, operand_y);
        // bigint_multiplication_textbook(&result, operand_x, operand_y);
        // bigint_multiplication_karatsuba(&result, operand_x, operand_y);
        // bigint_squaring_textbook(&result, operand_x);
        // bigint_squaring_karatsuba(&result, operand_x);
        // bigint_division_binary_long(&result, &result2, operand_x, operand_y);
        // bigint_division_word_long(&result, &result2, operand_x, operand_y);
        // bigint_reduction_barrett_pre_computed(&result, operand_y);
        // bigint_reduction_barrett(&result2, operand_x, operand_y, result);
        // bigint_exponentiation_modular_left_to_right(&result, operand_x, operand_y, operand_z); //! LEAK -- clear
        // bigint_exponentiation_modular_montgomery_ladder(&result, operand_x, operand_y, operand_z);

        /* print */
        bigint_show_hex(result);
    }

    /* free memory */
    bigint_delete(&operand_x);
    bigint_delete(&operand_y);
    bigint_delete(&operand_z);
    bigint_delete(&result);
    bigint_delete(&result2);

    /* file close */
    fclose(file_x);
    fclose(file_y);
    fclose(file_z); 
}

/**
 * @brief verification code using detail test vectors.
 * 
 */
void bigint_bit_test()
{
    /* file open */
    FILE* file_x = fopen("test/bit_test_vectors/tv_x.txt", "r");
    FILE* file_y = fopen("test/bit_test_vectors/tv_y.txt", "r");

    /* file open error */
    if(file_x == NULL || file_y == NULL) {
        perror("bigint_test: file open error");
        exit(1);
    }

    /* init */
    Bigint *operand_x = NULL;
    Bigint *operand_y = NULL;
    Bigint *operand_z = NULL;
    Bigint *result = NULL;
    Bigint *result2 = NULL;

    /* allocate */
    bigint_new(&operand_x, 1);
    bigint_new(&operand_y, 1);
    bigint_new(&operand_z, 1);
    bigint_new(&result, 1);
    bigint_new(&result2, 1);

    /* test count */
    uint32_t count = 4225;

    /* test start */
	while(count--)
	{
        /* read */
    	bigint_read(&operand_x, file_x);
        bigint_read(&operand_y, file_y);

        /* set sign */
        if(bigint_is_zero(operand_x) == FALSE) operand_x->sign = NEGATIVE;
        if(bigint_is_zero(operand_y) == FALSE) operand_y->sign = NEGATIVE;

        /* operation */
    	bigint_multiplication_karatsuba(&result, operand_x, operand_y);

        /* print */
        bigint_show_hex(result);
	}
    /*-----------------------------------------------------------*/

    /* free memory */
    bigint_delete(&operand_x);
    bigint_delete(&operand_y);
    bigint_delete(&operand_z);
    bigint_delete(&result);
    bigint_delete(&result2);

    /* file close */
    fclose(file_x);
    fclose(file_y);
}