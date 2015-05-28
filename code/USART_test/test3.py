import serial
import random

ser = serial.Serial("/dev/ttyACM0", 230400, timeout=1)

ok = 0
ko = 0
nb_bytes = 1000000
for i in range(nb_bytes):
    char = random.randint(0,255)
    ser.write([char])
    result = ser.read()
    if len(result) > 0 and result[0] == char:
        ok += 1
    else:
        ko += 1

print("nb_bytes = {}".format(nb_bytes))
print("ok = {}".format(ok))
print("ko = {}".format(ko))

