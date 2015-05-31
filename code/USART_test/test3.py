import serial
import random

ser = serial.Serial("/dev/ttyACM0", 115200, timeout=1, bytesize=8, stopbits=2)

def test_bunch(nb_bytes=100): 
    ok = 0
    ko = 0
    for i in range(nb_bytes):
        chars = [random.randint(0,255) for _ in range(181)]
        ser.write(chars)
        result = list(ser.read(181))
        if len(result) > 0 and result == chars:
            ok += 1
        else:
            ko += 1

    print("nb_chars = {}".format(nb_bytes))
    print("ok = {}".format(ok))
    print("ko = {}".format(ko))

def test_indiv(nb_bytes=100):
    ok = 0
    ko = 0
    for i in range(nb_bytes):
        char = random.randint(0,255)
        ser.write([char])
        result = ser.read()
        if len(result) > 0 and result[0] == char:
            ok += 1
        else:
            ko += 1

    print("nb_chars = {}".format(nb_bytes))
    print("ok = {}".format(ok))
    print("ko = {}".format(ko))
