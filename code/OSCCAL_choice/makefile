MMCU=atmega328

all: clean main upload 

main: main.o
	avr-gcc -g -mmcu=$(MMCU) -o test.elf test.o 

main.o:
	avr-gcc -g -Os -mmcu=$(MMCU) -c test.c

hex:
	avr-objcopy -j.text -j.data -O ihex test.elf test.hex

clean:
	-rm *.o *.elf *.hex

upload:
	avrdude -p $(MMCU) -c dragon_isp -P usb -B 4 -u -U flash:w:test.elf
