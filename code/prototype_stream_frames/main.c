#define F_CPU 7372800

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define USART_BAUDRATE 115200
#define BAUD_PRESCALE (((( F_CPU / 16) + ( USART_BAUDRATE / 2) ) / ( USART_BAUDRATE ) ) - 1)

typedef uint8_t   u8;
typedef uint16_t  u16;

#define NUM_WS2812     60 
#define NUM_LEDS      (NUM_WS2812*3)

#define START_DATA_TRANSMISSION   5
#define OK 1
#define KO 2

extern void output_grb(u8 * ptr, u16 count);

void set_color(u8 * p_buf, u8 led, u8 r, u8 g, u8 b)
{
	u16 index = 3*led;
	p_buf[index++] = g;
	p_buf[index++] = r;
	p_buf[index] = b;  
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

int main(void)
{
	u8 buf[NUM_LEDS];
	int i = 0;
	u8 byte = 0;

	DDRB = (1<<1);   // PB1

	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (0<<USBS0)|(3<<UCSZ00);

	UBRR0H = (unsigned char)(BAUD_PRESCALE>>8);
	UBRR0L = (unsigned char)BAUD_PRESCALE;

	OSCCAL=0x52;

	memset(buf, 0, sizeof(buf));

	while(1) 
	{
		output_grb(buf, sizeof(buf));

		send_byte(OK);

		byte = receive_byte();
		if(byte == START_DATA_TRANSMISSION) {
			for(i = 0; i<NUM_LEDS; i++) {
				buf[i] = receive_byte();
			}
		}
	}

	return 0;
}
