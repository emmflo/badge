import serial
import time
import pyglet
import itertools
from pyglet.image.codecs.pil import PILImageDecoder

ser = serial.Serial("/dev/ttyACM0", 230400, timeout=1)

def waitForByte(b):
    lol = None
    while lol != b:
    #print(lol)
        lol = ser.read()
        if len(lol) > 0:
            lol = lol[0]
    return lol

def waitForLine():
    lol = b''
    while lol == b'':
    #print(lol)
        lol = ser.readline()
    return lol

def sendVerifiedByte(b):
    lol = None
    while lol != b:
        ser.write([b])
        lol = ser.read()
        if len(lol) > 0:
            lol = lol[0]

def xor_it(data):
    temp = 0
    for elem in data:
        temp = elem ^ temp
    return temp

def sendFrame(frame):
    #char = ser.read(1) 
    #if len(char) == 0: char = -1
    #while char != ord('1'): 
    #    ser.write([ord('1')])
    #    char = ser.read(1) 
    #    if len(char) == 0: char = -1
    #ser.write(frame)
    ser.write([ord("1")])
    try:
        while ser.read(1)[0] != ord("1"): pass
    except (IndexError):
        pass
    check = xor_it(frame)
    ser.write(frame + [check])
    try:
        while ser.read(1)[0] != 6: pass
    except (IndexError):
        pass

def test():
    while True:
        sendFrame([20, 0, 10]*4 + [0, 40, 30]*4)
        sendFrame([0, 40, 12]*4 + [0, 20, 0]*4)
    
def loadPattern(pattern_image, percent=100):
    pattern_rawimage = pattern_image.get_image_data()
    format = 'GRB'
    pitch = pattern_rawimage.width * len(format)
    pattern_rawimage_array = bytearray(pattern_rawimage.get_data(format, pitch))
    pattern_rawimage_list = [int(i*(percent/100)) for i in list(pattern_rawimage_array)]
    lines = [list(pattern_rawimage_list[i:i+36]) for i in range(0, len(pattern_rawimage_list), 36)]
    lines[0] = list(itertools.chain.from_iterable([tuple(lines[0][i:i+3]) for i in range(0, len(lines[0]), 3)][::-1]))
    lines[2] = list(itertools.chain.from_iterable([tuple(lines[2][i:i+3]) for i in range(0, len(lines[2]), 3)][::-1]))
    lines[4] = list(itertools.chain.from_iterable([tuple(lines[4][i:i+3]) for i in range(0, len(lines[4]), 3)][::-1]))
    return list(itertools.chain.from_iterable(lines))

def loadImage(filename):
    return pyglet.image.load(filename, decoder=PILImageDecoder())

def main():
    path = "lol2.bmp"
    pattern_image = pyglet.image.load(path, decoder=PILImageDecoder())
    pattern_images = pyglet.image.ImageGrid(pattern_image, 1, 8)
    patterns = []
    for i in range(8):
        patterns.append(loadPattern(pattern_images[i], 10))

    count = 0
    global_count = 1
    start_time = time.time()
    while True:
        sendFrame(patterns[int(count)])
        #time.sleep(0.02) 
        if count >= 7:
            count = 0
        else:
            count += 0.25
        print(global_count/(time.time()-start_time))
        global_count += 1

if __name__ == '__main__':
    main()
