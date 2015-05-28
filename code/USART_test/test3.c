#define F_CPU 7372800

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define USART_BAUDRATE 230400
#define BAUD_PRESCALE (((( F_CPU / 16) + ( USART_BAUDRATE / 2) ) / ( USART_BAUDRATE ) ) - 1)

typedef uint8_t   u8;
typedef uint16_t  u16;

int main(void)
{
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);

	UBRR0H = (unsigned char)(BAUD_PRESCALE>>8);
	UBRR0L = (unsigned char)BAUD_PRESCALE;

	UCSR0B |= (1 << RXCIE0);

	OSCCAL=0x48;

	sei();

	//UDR0 = 's';

	while(1) {}
}

ISR(USART_RX_vect, ISR_BLOCK)
{
	u8 received_byte;
	received_byte = UDR0;
	UDR0 = received_byte;
}
