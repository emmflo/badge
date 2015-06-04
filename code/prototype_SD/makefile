MMCU=atmega328

all: clean main upload 

main: main.o
	avr-gcc -g -mmcu=$(MMCU) -o main.elf main.o 

main.o:
	avr-gcc -g -Os -mmcu=$(MMCU) -c main.c

hex:
	avr-objcopy -j.text -j.data -O ihex main.elf main.hex

clean:
	-rm *.o *.elf *.hex

upload:
	avrdude -p $(MMCU) -c dragon_isp -P usb -B 4 -u -U flash:w:main.elf 
