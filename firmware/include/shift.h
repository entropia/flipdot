#ifndef _SHIFT_H_
#define _SHIFT_H_

#include <stdint.h>
#include <stdbool.h>

void init_shift(void);

void shift_clear(void);
void shift_set(uint8_t row, bool high);

#endif
