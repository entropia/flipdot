#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>
#include <stdbool.h>

void set_speed(uint8_t speed);

void set_dot(uint8_t x, uint8_t y, bool set);
bool get_dot(uint8_t x, uint8_t y);
void force_dot(uint8_t x, uint8_t y, bool set);
void synchronize(void);
void clear(void);

#endif
