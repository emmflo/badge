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

#define START_TEXT_TRANSMISSION   4
#define START_DATA_TRANSMISSION   5
#define SET_BACKGROUND_COLOR      6
#define SET_TEXT_COLOR            7
#define SET_SLEEP_TIME            8
#define OK 1
#define OK_COMMAND 3
#define OK_TEXT    4
#define KO 2

#define MODE_TEXT                 4
#define MODE_DATA                 5

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

u8 non_bloquant_receive_byte()
{
	int i = 0;
	u8 byte = 0;
	while (( UCSR0A & (1 << RXC0 ) ) == 0 && i < 100) {
		i++;
	}
	byte = UDR0;
	if(i >= 10) {
		byte = 0;
	}
	return byte;
}

void display_char(u8 * p_buf, char c, int offset, u8 r, u8 g, u8 b) {
	if(offset < 12 && offset > -3) {

	int i = 0;
	int j = 0;
	u16 font_c = pgm_read_word(&font[c-' ']);

	u8 left_bound = 0;
	u8 right_bound = 3;

	if(offset == 10) { right_bound=2;}
	else if(offset == 11) { right_bound=1;}
	else if(offset == -1) { left_bound = 1;}
	else if(offset == -2) { left_bound = 2;}
	
	for(i=0; i<5; i++) {
		for(j=left_bound; j<right_bound; j++) {
			if(((font_c >> (i*3+j))  & 0x01)) {
				if(i%2 == 0) {
					set_color(p_buf, 59-offset-j-i*12, r, g, b);
				}
				else {
					set_color(p_buf, 59-(11-offset)+j-i*12, r, g, b);
				}
			}
		}
	}

	}
}

int scrolling_text(u8 * p_buf, char * text, u8 reset, int new_offset, int shift, u8 r, u8 g, u8 b) {
	static int offset = 12;
	int i=0;
	int o=0;
	char c = ' ';

	if(reset) {
		offset = new_offset;
	}
	offset = offset + shift;
	c = text[i];
	while(c != '\0') {
		o = offset + 4*i;
		if(o >= 12) {
			break;
		}
		display_char(p_buf, c,  o, r, g, b);
		i++;
		c = text[i];
	}
	if(o < -2) {
		offset = 12;
		return 1;
	}
	return 0;
}

void set_background_color(u8 * p_buf, u8 r, u8 g, u8 b) {
	int i = 0;
	for(i = 0; i < NUM_WS2812; i++) {
		set_color(p_buf, i, r, g, b);
	}
}

void delay_ms(int n) {
	while (n--) {

		_delay_ms(1);
	}
}

int main(void)
{
	u8 buf[NUM_LEDS];
	char text_buf[256] = {0};
	int i = 0;
	int r = 0;
	u8 byte = 0;
	u8 sleep_time = 0;
	u8 mode = MODE_TEXT;
	
	u8 t_r = 10;
	u8 t_g = 10;
	u8 t_b = 10;
	u8 b_r = 0;
	u8 b_g = 0;
	u8 b_b = 0;

	DDRB = (1<<1);   // PB1

	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	UCSR0C = (0<<USBS0)|(3<<UCSZ00);

	UBRR0H = (unsigned char)(BAUD_PRESCALE>>8);
	UBRR0L = (unsigned char)BAUD_PRESCALE;

	OSCCAL=0x52;

	memset(buf, 0, sizeof(buf));

	while(1) {
		if(mode == MODE_TEXT) {
			set_background_color(&buf, b_r, b_g, b_b);
			r = scrolling_text(&buf, text_buf, 0, 0, -1, t_r, t_g, t_b);
			output_grb(buf, sizeof(buf));
			if(r) {
				send_byte(OK_TEXT);
			}
			send_byte(OK);
			byte = non_bloquant_receive_byte();
		}
		else if(mode == MODE_DATA) {
			output_grb(buf, sizeof(buf));
			send_byte(OK);
			byte = receive_byte();
		}

		if(byte == START_DATA_TRANSMISSION) {
			send_byte(OK_COMMAND);
			mode = MODE_DATA;
			for(i = 0; i<NUM_LEDS; i++) {
				buf[i] = receive_byte();
			}
		}
		else if(byte == START_TEXT_TRANSMISSION) {
			i = 0;
			byte = '\0';
			send_byte(OK_COMMAND);
			while(byte != '\n' && i < 256) {
				byte = receive_byte();
				text_buf[i] = byte;
				i++;
			}
			text_buf[i-1] = '\0';
			r = scrolling_text(&buf, text_buf, 1, 13, -1, t_r, t_g, t_b);
			mode = MODE_TEXT;
		}
		else if(byte == SET_SLEEP_TIME) {
			send_byte(OK_COMMAND);
			sleep_time = receive_byte();
		}
		else if(byte == SET_TEXT_COLOR) {
			send_byte(OK_COMMAND);
			t_r = receive_byte();
			t_g = receive_byte();
			t_b = receive_byte();
		}
		else if(byte == SET_BACKGROUND_COLOR) {
			send_byte(OK_COMMAND);
			b_r = receive_byte();
			b_g = receive_byte();
			b_b = receive_byte();
		}

		if(sleep_time != 0) {
			delay_ms(sleep_time);
		}
	}

	return 0;
}
