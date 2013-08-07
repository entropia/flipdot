#include <stdint.h>

#include "display.h"

uint8_t char_0[]	= { 0x7C, 0x82, 0x92, 0x82, 0x7C, };
uint8_t char_1[]	= { 0x00, 0x84, 0xFE, 0x80, 0x00, };
uint8_t char_2[]	= { 0xC4, 0xA2, 0x92, 0x92, 0x8C, };
uint8_t char_3[]	= { 0x44, 0x82, 0x92, 0x92, 0x6C, };
uint8_t char_4[]	= { 0x30, 0x28, 0x24, 0xFE, 0x20, };
uint8_t char_5[]	= { 0x5E, 0x92, 0x92, 0x92, 0x62, };
uint8_t char_6[]	= { 0x78, 0x94, 0x92, 0x92, 0x60, };
uint8_t char_7[]	= { 0x02, 0x82, 0x62, 0x1A, 0x06, };
uint8_t char_8[]	= { 0x6C, 0x92, 0x92, 0x92, 0x6C, };
uint8_t char_9[]	= { 0x0C, 0x92, 0x92, 0x52, 0x3C, };
uint8_t char_colon[]	= { 0x00, 0x00, 0x48, 0x00, 0x00, };
uint8_t char_dot[]	= { 0x00, 0x00, 0x80, 0x00, 0x00, };

void draw_digit(uint8_t x, uint8_t y, char c) {
	uint8_t *digit;

	switch(c) {
		case '0':
			digit = char_0;
			break;
		case '1':
			digit = char_1;
			break;
		case '2':
			digit = char_2;
			break;
		case '3':
			digit = char_3;
			break;
		case '4':
			digit = char_4;
			break;
		case '5':
			digit = char_5;
			break;
		case '6':
			digit = char_6;
			break;
		case '7':
			digit = char_7;
			break;
		case '8':
			digit = char_8;
			break;
		case '9':
			digit = char_9;
			break;
		case ':':
			digit = char_colon;
			break;
		case '.':
			digit = char_dot;
			break;
		default:
			return;
	}

	for(uint8_t xo = 0; xo < 5; xo++) {
		uint8_t map = digit[xo];

		for(uint8_t yo = 0; yo < 8; yo++) {
			draw_dot(x + xo, y + yo, map & 1);
			map >>= 1;
		}
	}
}

void puts(uint8_t x, uint8_t y, char *s) {
	while(*s) {
		if(*s == ' ')
			x += 5;
		else
			draw_digit(x, y, *s);

		x += 6;

		s++;
	}
}
