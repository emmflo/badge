import test

path = "lol2.bmp"
pattern_image = test.pyglet.image.load(path, decoder=test.PILImageDecoder())
pattern_images = test.pyglet.image.ImageGrid(pattern_image, 1, 8)
patterns = []
for i in range(8):
    patterns.append(test.loadPattern(pattern_images[i], 10))

count = 0
global_count = 1

for i in range(16):
    test.sendVerifiedByte(ord('s'))
    #print(test.ser.readline())
    #print(test.ser.read())
    print(test.waitForLine())
    #print(test.waitForByte(ord('o')))
    for j in range(100):
        #print(j)
        test.sendFrame(patterns[int(count)])
        #time.sleep(0.02) 
        if count >= 7:
            count = 0
        else:
            count += 0.25
        global_count += 1
    test.sendVerifiedByte(ord('d'))
    #print(test.ser.readline())
    print(test.waitForLine())

