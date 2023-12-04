import secrets
import time

# init values
operand_x = secrets.randbits(2048)
operand_y = secrets.randbits(2048)
operand_y_half = secrets.randbits(1024)
operand_z = secrets.randbits(2048)
count = 100 # operation count

# time check : multiplication
start = time.time()
for i in range(count):
    result = operand_x * operand_y
end = time.time()
time_result = round(end - start, 6)
print(f"time multiplication : {time_result}")

# time check : squaring
start = time.time()
for i in range(count):
    result = operand_x ** 2
end = time.time()
time_result = round(end - start, 6)
print(f"time squaring       : {time_result}")

# time check : division
start = time.time()
for i in range(count):
    result = operand_x // operand_y_half
end = time.time()
time_result = round(end - start, 6)
print(f"time division       : {time_result}")

# time check : redcution
start = time.time()
for i in range(count):
    result = operand_x % operand_y_half
end = time.time()
time_result = round(end - start, 6)
print(f"time reduction      : {time_result}")

# time check : exponetiation
start = time.time()
result = pow(operand_x, operand_y, operand_z)
end = time.time()
time_result = round(end - start, 6)
print(f"time exponetiation  : {time_result}")