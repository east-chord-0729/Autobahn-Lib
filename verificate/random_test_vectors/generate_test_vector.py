import secrets

def generate_test_vector():
    
    # operand test vectors files
    file_operand_x = open("operand_x8.txt", 'w')
    file_operand_y = open("operand_y8.txt", 'w')
    file_operand_z = open("operand_z8.txt", 'w')

    # result of operantion files
    file_addition = open("addition8.txt", 'w')
    file_subtraction = open("subtraction8.txt", 'w')
    file_multiplication = open("multiplication8.txt", 'w')
    file_squaring = open("squaring8.txt", 'w')
    file_division_quotient = open("quotient8.txt", 'w')
    file_division_remainder = open("remainder8.txt", 'w')
    file_exponentiation = open("exponentiation8.txt", 'w')

    print("generating test vectors...")

    # genrate
    for i in range(0, 1000):

        # operand
        operand_x = secrets.randbits(256)
        operand_y = secrets.randbits(192)
        operand_z = secrets.randbits(256)

        # result of operation
        result_add = operand_x + operand_y
        result_sub = operand_x - operand_y
        result_mul = operand_x * operand_y
        result_sqr = operand_x * operand_x
        result_quo = operand_x // operand_y
        result_rem = operand_x % operand_y
        result_exp = pow(operand_x, operand_y, operand_z)

        # int to hex string
        hex_string_x = "%x\n" % operand_x
        hex_string_y = "%x\n" % operand_y
        hex_string_z = "%x\n" % operand_z
        hex_string_add = "%x\n" % result_add
        hex_string_sub = "%x\n" % result_sub
        hex_string_mul = "%x\n" % result_mul
        hex_string_sqr = "%x\n" % result_sqr
        hex_string_quo = "%x\n" % result_quo
        hex_string_rem = "%x\n" % result_rem
        hex_string_exp = "%x\n" % result_exp

        # write
        file_operand_x.write(hex_string_x)
        file_operand_y.write(hex_string_y)
        file_operand_z.write(hex_string_z)
        file_addition.write(hex_string_add)
        file_subtraction.write(hex_string_sub)
        file_multiplication.write(hex_string_mul)
        file_squaring.write(hex_string_sqr)
        file_division_quotient.write(hex_string_quo)
        file_division_remainder.write(hex_string_rem)
        file_exponentiation.write(hex_string_exp)

    # file close 
    file_operand_x.close()
    file_operand_y.close()
    file_operand_z.close()
    file_addition.close()
    file_subtraction.close()
    file_multiplication.close()
    file_squaring.close()
    file_division_quotient.close()
    file_division_remainder.close()
    file_exponentiation.close()

# main
generate_test_vector()