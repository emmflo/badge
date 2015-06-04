import pyglet

from pyglet.image.codecs.pil import PILImageDecoder
import bluetooth

window = pyglet.window.Window()
img = pyglet.image.load('dot.png', decoder=PILImageDecoder())
sprite = pyglet.sprite.Sprite(img)
sprite.scale = 4
batch = pyglet.graphics.Batch()
first = True

def xor_it(data):
    temp = 0
    for elem in data:
        temp = elem ^ temp
    return temp

class Badge:
    def __init__(self, batch, device, baud, timeout):
        self.img_pixel = pyglet.image.load('pixel.bmp', decoder=PILImageDecoder())
        self.frame = []
        self.batch = batch
        self.pixel_grid = []
        self.pixel_data = []
        #self.ser = serial.Serial(device, baud, timeout=timeout, bytesize=8, stopbits=2)
        #self.ser = serial.Serial("/dev/ttyACM0", 115200, timeout=1, bytesize=8, stopbits=2)
        self.socket = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
        self.socket.connect(('30:15:01:07:02:21', 1))
        pyglet.clock.schedule_interval(self.sendFrameNoCheck, 1/30)
        self.first = True

    def makeGrid(self, start_x, start_y, end_x, end_y, rows, columns, zigzag, top):
        space_w = abs(end_x - start_x) // (columns - 1)
        space_h = abs(end_y - start_y) // (rows - 1)
        pixel_grid = []
        num_pixels = rows * columns
        for i in range(rows):
            for j in range(columns):
                if zigzag and i % 2 == 0:
                    pixel_grid.append(((start_x + (columns - 1) * space_w) - j * space_w, start_y + i * space_h))
                else:
                    pixel_grid.append((start_x + j * space_w, start_y + i * space_h))

        self.pixel_grid = pixel_grid

        self.sprites_pixel = []
        for elem in self.pixel_grid:
            self.sprites_pixel.append(pyglet.sprite.Sprite(self.img_pixel, batch=self.batch))
            self.sprites_pixel[-1].set_position(*elem)

        return pixel_grid

    def getPixelData(self):

        #image_data = pyglet.image.get_buffer_manager().get_color_buffer().get_image_data()
        #data = image_data.get_data('RGB', image_data.width * 3)
        #pixel_data = []
        #for pixel in pixel_grid :
        #    pos = pixel[0] * pixel[1] * 3
        #    for i in range(3):
        #        pixel_data.append(data[pos + i])
        #return pixel_data
        
        pixel_data = []
        buffer_manager = pyglet.image.get_buffer_manager().get_color_buffer()
        for pixel in self.pixel_grid:
            data = buffer_manager.get_region(pixel[0], pixel[1], 1, 1).get_image_data().get_data('RGB', 3)
            pixel_data.append(data[1])
            pixel_data.append(data[0])
            pixel_data.append(data[2])
        self.pixel_data = pixel_data
        self.frame = [int(i*(10/100)) for i in list(self.pixel_data)]
        #print(self.frame)
        return pixel_data

    def sendSomething(self, thing):
        if self.first:
            self.first = False
        else:
            while self.socket.recv(1)[0] != 1: pass
        self.socket.send(bytes(thing[:1]))
        while self.socket.recv(1)[0] != 3: pass
        self.socket.send(bytes(thing[1:]))

    def setBackgroundColor(self, r, g, b):
        self.sendSomething([6] + [r] + [g] + [b])

    def setTextColor(self, r, g, b):
        self.sendSomething([7] + [r] + [g] + [b])

    def setSleepTime(self, time):
        self.sendSomething([8] + [time])

    def sendFrame(self, dt):
        frame = self.frame
        self.sendSomething([5] + frame)

    def sendFrameNoCheck(self, dt):
        frame = self.frame
        if self.first:
            self.first = False
        else:
            while self.socket.recv(1)[0] != 1: pass
        self.socket.send(bytes([5]))
        #time.sleep(0.001)
        self.socket.send(bytes(frame))


badge = Badge(batch, "/dev/ttyACM0", 115200, 1)
#badge = Badge(batch, "/dev/ttyACM0", 38400, 1)
#badge.ser.write([ord('2')])


#pixel_grid = [(20, 20), (20, 40), (20, 60)]
pixel_grid = badge.makeGrid(20, 115, 620, 365, 5, 12, True, False)

@window.event
def on_mouse_motion(x, y, dx, dy):
    sprite.set_position(x-(sprite.width // 2), y-(sprite.height // 2))

@window.event
def on_mouse_press(x, y, buttons, modifiers):
    if buttons & pyglet.window.mouse.LEFT:
        #badge.sendFrame(0)
        pass
 
@window.event
def on_draw():
    global frame
    window.clear()
    sprite.draw()
    badge.getPixelData()
    #badge.sendFrame(0)
    batch.draw()


#pyglet.clock.set_fps_limit(20)

pyglet.app.run()
