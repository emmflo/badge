#define F_CPU   7372800

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#define USART_BAUDRATE 115200 
#define BAUD_PRESCALE (((( F_CPU / 16) + ( USART_BAUDRATE / 2) ) / ( USART_BAUDRATE ) ) - 1)
//#define BAUD_PRESCALE 0x0001

void uart_putchar(char c, FILE *stream) {
	    if (c == '\n') {
		            uart_putchar('\r', stream);
			        }
	        loop_until_bit_is_set(UCSR0A, UDRE0);
		    UDR0 = c;
}

char uart_getchar(FILE *stream) {
	    loop_until_bit_is_set(UCSR0A, RXC0); /* Wait until data exists. */
	        return UDR0;
}

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
//FILE uart_io FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);

int main(void)
{
	int step = 0;
	int i = 0;
	int orig = OSCCAL;
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);

	UBRR0H = (unsigned char)(BAUD_PRESCALE>>8);
	UBRR0L = (unsigned char)BAUD_PRESCALE;

	UCSR0B |= (1 << RXCIE0);
	DDRB = (1<<0);   // bit 4 is our output
	sei();

	stdout = &uart_output;
	stdin  = &uart_input;

	for (step = 0x00; step <= 0xFF; step += 0x01) {
		//OSCCAL = 0x95;		//2MHz RC oscillator good guess
		OSCCAL = step;		//do string at this calibration:
		printf("\r\nUsing OSCCAL=%02X:", step);
		printf("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
		_delay_ms(100);		//
		for (i = 0; i < 10; i++) {
			PORTB ^= (1 << 0);	//always nice to flash an LED
			_delay_ms(10);
		}
	}
	return 0;
}
