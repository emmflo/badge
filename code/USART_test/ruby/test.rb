require 'rubygems'
require 'serialport'
require 'timeout'

ser = SerialPort.new("/dev/ttyACM2", 1200, 8, 1, SerialPort::NONE)
ser2 = SerialPort.new("/dev/ttyACM2", 1200, 8, 1, SerialPort::NONE)

prng = Random.new
ary = "wwwwwwwwwweeeeeeeeeerrrrrrrrrrttttttttttyyyyyyyyyyuuuuuuuuuuiiiiiiiiiioooooooooopppppppppppqqqqqqqqqq"
print ary
print "\n"

ser.write(ary)
sleep(1)
Timeout.timeout(1) do
	ary2 = ser2.read(100)
end
print ary2
print "\n"
