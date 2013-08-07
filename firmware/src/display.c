#include <stdint.h>
#include <stdbool.h>

#include "fp2800.h"
#include "shift.h"

#include "config.h"

static bool current[WIDTH][HEIGHT];

static uint8_t resolve_module(uint8_t x) {
	uint8_t module_no = x / 28;

	return module_order[module_no];
}

static uint8_t strobe_speed = 2;

void set_speed(uint8_t speed) {
	if(speed <= 2)
		strobe_speed = speed;
}

void force_dot(uint8_t x, uint8_t y, bool set) {
	if(!(x < WIDTH && y < HEIGHT))
		return;

	shift_set(y, set);

	fp2800_select_col(x % 28);
	fp2800_set_polarity(!set);

	fp2800_strobe(strobe_speed, resolve_module(x));

	shift_clear();

	current[x][y] = set;
}

void set_dot(uint8_t x, uint8_t y, bool set) {
	if(!(x < WIDTH && y < HEIGHT))
		return;

	if(current[x][y] == set)
		return;

	force_dot(x, y, set);
}

bool get_dot(uint8_t x, uint8_t y) {
	if(x < WIDTH && y < HEIGHT)
		return current[x][y];

	return false;
}

void synchronize(void) {
	for(uint8_t x = 0; x < WIDTH; x++)
		for(uint8_t y = 0; y < HEIGHT; y++)
			force_dot(x, y, current[x][y]);
}

void clear(void) {
	for(uint8_t x = 0; x < WIDTH; x++)
		for(uint8_t y = 0; y < HEIGHT; y++)
			set_dot(x, y, false);

}
