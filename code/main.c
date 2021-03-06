//
// AVR_2812
// 6 WS2812B LEDs
// 8MHz internal osc
//

//#define F_CPU   8000000
#define F_CPU 7372800

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>

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

// declaration of our ASM function
extern void output_grb(u8 * ptr, u16 count);

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

void set_color(u8 * p_buf, u8 led, u8 r, u8 g, u8 b)
{
	u16 index = 3*led;
	p_buf[index++] = g;
	p_buf[index++] = r;
	p_buf[index] = b;  
}

int main(void)
{
	u8 buf[NUM_LEDS+1];
	u8 local_state;
	u8 local_transfert;
  
	DDRB = (1<<1);   // bit 4 is our output

	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);

	UBRR0H = (unsigned char)(BAUD_PRESCALE>>8);
	UBRR0L = (unsigned char)BAUD_PRESCALE;

	UCSR0B |= (1 << RXCIE0);

	OSCCAL=0x48;

	sei();
    
	memset(buf, 10, sizeof(buf));
	memset(temp_buf, 0, sizeof(temp_buf));

	output_grb(buf, sizeof(buf));
	_delay_ms(1000);

	memset(buf, 0, sizeof(buf));

	u8 count = 0;
	u8 color = 0;
        
	while(1)
	{
		//output_grb(buf, sizeof(buf));
		local_state = state;
      		switch (local_state)
		{
			case FRAME:
				local_transfert = transfert;
				if(local_transfert == 2)
				{
					cli();
					memcpy(&buf, &temp_buf, sizeof(buf));
					sei();
					transfert = 0;
					local_transfert = 0;
					state = '0';
					output_grb(buf, sizeof(buf));
					while (( UCSR0A & (1 << UDRE0 ) ) == 0) {};
					UDR0 = DONE;
					//_delay_ms(100);
				}
			break;
			case TEST:
				memset(buf, 50, sizeof(buf));
				state = '0';
			break;
			case NONE:
				memset(buf, 0, sizeof(buf));
				state = '0';
			break;
			case FULL_TEST:
				if(color == 0)
					set_color(&buf, count, 20, 0, 0);
				else if(color == 1)
					set_color(&buf, count, 0, 20, 0);
				else if(color == 2)
					set_color(&buf, count, 0, 0, 20);
				if(count < NUM_WS2812)
				{
					count++;
				}
				else
				{
					count = 0;
					memset(buf, 10, sizeof(buf));
					if(color < 2)
					{
						color++;
					}
					else
					{
						color = 0;
					}
				}
			break;
			
		}
		_delay_ms(50);
	}
}

ISR(USART_RX_vect, ISR_BLOCK)
{
	u8 received_byte;
	received_byte = UDR0;
	UDR0 = received_byte;
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
			}
			else
			{
				transfert = 0;
				iter_buff = 0;
			}

		}
	}
	else if(transfert == 0)
	{
		state = received_byte;
		switch(state)
		{
			case FRAME:
				transfert = 1;
			break;
		}
	}

}
