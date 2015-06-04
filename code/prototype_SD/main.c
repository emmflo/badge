#define F_CPU 7372800

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <avr/pgmspace.h>

#include "../font/font.h"

#define USART_BAUDRATE 115200
#define BAUD_PRESCALE (((( F_CPU / 16) + ( USART_BAUDRATE / 2) ) / ( USART_BAUDRATE ) ) - 1)

typedef uint8_t   u8;
typedef uint16_t  u16;

#define NUM_WS2812     60 
#define NUM_LEDS      (NUM_WS2812*3)

#define CS (1<<PB2)
#define MOSI (1<<PB3)
#define MISO (1<<PB4)
#define SCK (1<<PB5)
#define CS_DDR DDRB
#define CS_ENABLE() (PORTB &= ~CS)
#define CS_DISABLE() (PORTB |= CS)

void SPI_init() {
	CS_DDR |= CS; // SD card circuit select as output
	DDRB |= MOSI + SCK; // MOSI and SCK as outputs
	PORTB |= MISO; // pullup in MISO, might not be needed

	// Enable SPI, master, set clock rate fck/128
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0) | (1<<SPR1);
}

unsigned char SPI_write(unsigned char ch) {
	SPDR = ch;
	while(!(SPSR & (1<<SPIF))) {}       
	return SPDR;
}

void send_byte(u8 byte)
{
	while (( UCSR0A & (1 << UDRE0 ) ) == 0) {};
	UDR0 = byte;
}

u8 receive_byte()
{
	u8 byte = 0;
	while (( UCSR0A & (1 << RXC0 ) ) == 0) {};
	byte = UDR0;
	return byte;
}

void uwrite_hex(unsigned char n) {
	if(((n>>4) & 15) < 10)
		send_byte('0' + ((n>>4)&15));
	else
		send_byte('A' + ((n>>4)&15) - 10);
	n <<= 4;
	if(((n>>4) & 15) < 10)
		send_byte('0' + ((n>>4)&15));
	else
		send_byte('A' + ((n>>4)&15) - 10);
}

void uwrite_str(char *str) {
	char i;

	for(i=0; str[i]; i++)
		send_byte(str[i]);
}

void SD_command(unsigned char cmd, unsigned long arg, 
		unsigned char crc, unsigned char read) {
	unsigned char i, buffer[8];

	CS_ENABLE();
	SPI_write(cmd);
	SPI_write(arg>>24);
	SPI_write(arg>>16);
	SPI_write(arg>>8);
	SPI_write(arg);
	SPI_write(crc);

	for(i=0; i<read; i++) {
		buffer[i] = SPI_write(0xFF);
		uwrite_hex(buffer[i]);
	}

	CS_DISABLE();               

	uwrite_str("\r\n");

	// print out read bytes
}

int main(void)
{
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (3<<UCSZ00);

	UBRR0H = (unsigned char)(BAUD_PRESCALE>>8);
	UBRR0L = (unsigned char)BAUD_PRESCALE;

	OSCCAL=0x52;

	SPI_init();


	while(1) {
		switch(receive_byte()) {
			case '1':
				SD_command(0x40, 0x00000000, 0x95, 8);
				break;
			case '2':
				SD_command(0x41, 0x00000000, 0xFF, 8);
				break;
			case '3':
				SD_command(0x50, 0x00000200, 0xFF, 8);
				break;
		}
	}       
}
