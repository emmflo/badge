import time
import pyglet
import itertools
import bluetooth
import argparse
from pyglet.image.codecs.pil import PILImageDecoder

#ser = serial.Serial("/dev/rfcomm0", 9600)
socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
socket.connect(('30:15:01:07:02:21', 1))
first = True

def sendSomething(thing):
    global first
    if first:
        first = False
    else:
        while socket.recv(1)[0] != 1: pass
    socket.send(bytes(thing[:1]))
    while socket.recv(1)[0] != 3: pass
    socket.send(bytes(thing[1:]))

def setBackgroundColor(r, g, b):
    sendSomething([6] + [r] + [g] + [b])

def setTextColor(r, g, b):
    sendSomething([7] + [r] + [g] + [b])

def setSleepTime(time):
    sendSomething([8] + [time])

def sendFrame(frame):
    sendSomething([5] + frame)

def sendFrameNoCheck(frame):
    global first
    if first:
        first = False
    else:
        while socket.recv(1)[0] != 1: pass
    socket.send(bytes([5]))
    #time.sleep(0.001)
    socket.send(bytes(frame))


def sendText(text):
    sendSomething([4] + [ord(x) for x in text])

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

def main(path, inc):
    pattern_image = pyglet.image.load(path, decoder=PILImageDecoder())
    num_frames = int(pattern_image.width/12)
    pattern_images = pyglet.image.ImageGrid(pattern_image, 1, num_frames)
    patterns = []
    for i in range(num_frames):
        patterns.append(loadPattern(pattern_images[i], 10))

    #setSleepTime(0);

    count = 0
    global_count = 1
    start_time = time.time()
    while True:
        sendFrame(patterns[int(count)])
        #time.sleep(1/10) 
        if count >= num_frames-1:
            count = 0
        else:
            count += inc
        print(global_count/(time.time()-start_time))
        global_count += 1

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Stream animation to the Bagde.')
    parser.add_argument('filename', default="lol2.bmp", nargs='?')
    parser.add_argument('increment', type=float, default=1.0, nargs='?')
    args = parser.parse_args()
    path = args.filename
    inc = args.increment
    main(path, inc)
