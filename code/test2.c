// 8MHz internal osc
//

//#define F_CPU   8000000
#define F_CPU 7372800

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

//#define USART_BAUDRATE 9600
//#define USART_BAUDRATE 38400 
#define USART_BAUDRATE 230400
#define BAUD_PRESCALE (((( F_CPU / 16) + ( USART_BAUDRATE / 2) ) / ( USART_BAUDRATE ) ) - 1)

typedef uint8_t   u8;
typedef uint16_t  u16;

#define NUM_WS2812     60 
//#define NUM_WS2812    8
#define NUM_LEDS      (NUM_WS2812*3)

#define FRAME         '1'
#define NONE          '2'
#define TEST	      '3'
#define FULL_TEST     '4'
#define DONE	       6   //ACK

volatile u8 temp_buf[NUM_LEDS+1];
volatile u8 transfert = 0;
volatile u8 state = FULL_TEST;
volatile int iter_buff = 0;
volatile int received = 0;
volatile int dropped = 0;

// declaration of our ASM function
//extern void output_grb(u8 * ptr, u16 count);

u8 xor_it(u8 * p_buf)
{
	u8 temp = 0;
	int i = 0;
	for(i = 0; i <= NUM_LEDS; i++)
	{
		temp = temp ^ p_buf[i];
	}
	return temp;

}

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

	u8 buf[NUM_LEDS+1];
	u8 local_state;
	u8 local_transfert;
  
	//DDRB = (1<<1);   // bit 4 is our output

	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);

	UBRR0H = (unsigned char)(BAUD_PRESCALE>>8);
	UBRR0L = (unsigned char)BAUD_PRESCALE;

	UCSR0B |= (1 << RXCIE0);

	OSCCAL=0x48;

	sei();
    
	memset(buf, 0, sizeof(buf));
	memset(temp_buf, 0, sizeof(temp_buf));

	u8 count = 0;
	u8 color = 0;

	stdout = &uart_output;
	stdin  = &uart_input;

	//printf("Test\r\n");

	for (step = 0x41; step <= 0x50; step += 0x01) {
		while (state != 's') {};
		received = 0;
		dropped = 0;
		printf("Using OSCCAL=%02X\r\n", step);
		OSCCAL = step;		//do string at this calibration:
		while (( UCSR0A & (1 << UDRE0 ) ) == 0) {};
		//UDR0 = 'o';
		while (state != 'd') {};
		//while (( UCSR0A & (1 << UDRE0 ) ) == 0) {};
		OSCCAL = 0x48;		//2MHz RC oscillator good guess
		printf("dropped = %d; received = %d\r\n", dropped, received);

	}
	return 0;

}

ISR(USART_RX_vect, ISR_BLOCK)
{
	u8 received_byte;
	received_byte = UDR0;
	if(transfert == 1)
	{
		temp_buf[iter_buff] = received_byte;
		if(iter_buff <= NUM_LEDS)
		{
			iter_buff++;
		}
		else
		{
			if(xor_it(temp_buf) == 0x00)
			{
				iter_buff = 0;
				transfert = 2;
				received++;
			}
			else
			{
				transfert = 0;
				iter_buff = 0;
				dropped++;
			}
			UDR0 = DONE;

		}
	}
	else if(transfert == 0)
	{
		state = received_byte;
		switch(state)
		{
			case FRAME:
				UDR0 = received_byte;
				transfert = 1;
				break;
			case 's':
			case 'd':
				UDR0 = received_byte;
				break;
		}
	}

}
